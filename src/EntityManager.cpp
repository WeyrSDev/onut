// onut includes
#include <onut/Collider2DComponent.h>
#include <onut/Camera2DComponent.h>
#include <onut/Component.h>
#include <onut/Entity.h>
#include <onut/EntityManager.h>
#include <onut/Renderer.h>
#include <onut/SpriteBatch.h>
#include <onut/Timing.h>

// Third parties
#include <Box2D/Box2D.h>

OEntityManagerRef oEntityManager;

namespace onut
{
    class Physic2DContactListener : public b2ContactListener
    {
    public:
        Physic2DContactListener(EntityManager* pEntityManager)
            : m_pEntityManager(pEntityManager)
        {
        }

    private:
        void BeginContact(b2Contact* contact) override
        {
            m_pEntityManager->begin2DContact(contact);
        }

        void EndContact(b2Contact* contact) override
        {
            m_pEntityManager->end2DContact(contact);
        }

        EntityManager* m_pEntityManager;
    };

    OEntityManagerRef EntityManager::create()
    {
        return std::shared_ptr<EntityManager>(new EntityManager());
    }

    EntityManager::EntityManager()
    {
        m_pPhysic2DWorld = new b2World(b2Vec2(0, 0));
        m_pPhysic2DContactListener = new Physic2DContactListener(this);
        m_pPhysic2DWorld->SetContactListener(m_pPhysic2DContactListener);
    }

    EntityManager::~EntityManager()
    {
        delete m_pPhysic2DWorld;
        delete m_pPhysic2DContactListener;
    }

    void EntityManager::addEntity(const OEntityRef& pEntity)
    {
        if (pEntity->m_pEntityManager)
        {
            auto pEntityRef = pEntity;
            pEntityRef->m_pEntityManager->removeEntity(pEntityRef);
            pEntityRef->m_pEntityManager = OThis;
            m_entities.push_back(pEntityRef);
        }
        else
        {
            pEntity->m_pEntityManager = OThis;
            m_entities.push_back(pEntity);
        }
    }

    void EntityManager::removeEntity(const OEntityRef& pEntity)
    {
        // Remove components
        if (pEntity->isEnabled() && !pEntity->isStatic())
        {
            for (auto& pComponent : pEntity->m_components)
            {
                if (pComponent->isEnabled())
                {
                    addComponentAction(pComponent, m_componentUpdates, ComponentAction::Action::Remove);
                }
            }
        }
        if (pEntity->isVisible())
        {
            for (auto& pComponent : pEntity->m_components)
            {
                addComponentAction(pComponent, m_componentRenders, ComponentAction::Action::Remove);
            }
        }

        // Remove entity
        for (auto it = m_entities.begin(); it != m_entities.end(); ++it)
        {
            if (*it == pEntity)
            {
                m_entities.erase(it);
                pEntity->m_pEntityManager = nullptr;
                return;
            }
        }
    }

    void EntityManager::addComponentAction(const OComponentRef& pComponent, Components& list, ComponentAction::Action action)
    {
        m_componentActions.push_back({action, pComponent, &list});
    }

    void EntityManager::performComponentActions()
    {
        for (auto& pComponent : m_componentJustCreated)
        {
            pComponent->onCreate();
        }
        m_componentJustCreated.clear();
        for (auto& componentAction : m_componentActions)
        {
            switch (componentAction.action)
            {
                case ComponentAction::Action::Add:
                    addComponent(componentAction.pComponent, *componentAction.pTargetList);
                    break;
                case ComponentAction::Action::Remove:
                    removeComponent(componentAction.pComponent, *componentAction.pTargetList);
                    break;
            }
        }
        m_componentActions.clear();
    }

    void EntityManager::addComponent(const OComponentRef& pComponent, Components& to)
    {
        to.push_back(pComponent);
    }

    void EntityManager::removeComponent(const OComponentRef& pComponent, Components& from)
    {
        for (auto it = from.begin(); it != from.end(); ++it)
        {
            if (*it == pComponent)
            {
                from.erase(it);
                return;
            }
        }
    }

    void EntityManager::setActiveCamera2D(const OCamera2DComponentRef& pActiveCamera2D)
    {
        m_pActiveCamera2D = pActiveCamera2D;
    }

    void EntityManager::performContacts()
    {
        auto contacts = m_contact2Ds;
        m_contact2Ds.clear();
        for (auto& contact2D : contacts)
        {
            switch (contact2D.type)
            {
                case Contact2D::Type::Begin:
                    contact2D.pColliderA->getEntity()->onTriggerEnter(contact2D.pColliderB);
                    contact2D.pColliderB->getEntity()->onTriggerEnter(contact2D.pColliderA);
                    break;
                case Contact2D::Type::End:
                    contact2D.pColliderA->getEntity()->onTriggerLeave(contact2D.pColliderB);
                    contact2D.pColliderB->getEntity()->onTriggerLeave(contact2D.pColliderA);
                    break;
            }
        }
    }

    void EntityManager::update()
    {
        performComponentActions();
        m_pPhysic2DWorld->Step(ODT, 6, 2);
        performContacts();
        for (auto& pComponent : m_componentUpdates)
        {
            pComponent->onUpdate();
        }
        performComponentActions();
    }

    void EntityManager::render()
    {
        for (auto& pComponent : m_componentRenders)
        {
            pComponent->onRender();
        }

        Matrix transform;
        if (m_pActiveCamera2D)
        {
            if (m_pActiveCamera2D->getClear())
            {
                oRenderer->clear(m_pActiveCamera2D->getClearColor());
            }
            transform = m_pActiveCamera2D->getEntity()->getWorldTransform().Invert();
            transform *= Matrix::CreateScale(m_pActiveCamera2D->getZoom());
            transform *= Matrix::CreateTranslation(OScreenCenterf);
        }
        oSpriteBatch->begin(transform);
        for (auto& pEntity : m_entities)
        {
            if (!pEntity->getParent() && pEntity->isVisible())
            {
                pEntity->render2d();
            }
        }
        oSpriteBatch->end();
    }

    OEntityRef EntityManager::findEntity(const std::string& name) const
    {
        for (auto& pEntity : m_entities)
        {
            if (pEntity->getName() == name) return pEntity;
        }
        return nullptr;
    }

    b2World* EntityManager::getPhysic2DWorld() const
    {
        return m_pPhysic2DWorld;
    }

    void EntityManager::begin2DContact(b2Contact* pContact)
    {
        b2Fixture* pFixtureA = pContact->GetFixtureA();
        b2Fixture* pFixtureB = pContact->GetFixtureB();
        if (pFixtureA == pFixtureB) return;

        // Make sure only one of the fixtures was a sensor
        bool sensorA = pFixtureA->IsSensor();
        bool sensorB = pFixtureB->IsSensor();
        if (!(sensorA ^ sensorB)) return;

        auto* pColliderA = static_cast<Collider2DComponent*>(pFixtureA->GetBody()->GetUserData());
        auto* pColliderB = static_cast<Collider2DComponent*>(pFixtureB->GetBody()->GetUserData());

        m_contact2Ds.push_back({Contact2D::Type::Begin,
                               OStaticCast<Collider2DComponent>(pColliderA->shared_from_this()),
                               OStaticCast<Collider2DComponent>(pColliderB->shared_from_this())});
    }

    void EntityManager::end2DContact(b2Contact* pContact)
    {
        b2Fixture* pFixtureA = pContact->GetFixtureA();
        b2Fixture* pFixtureB = pContact->GetFixtureB();
        if (pFixtureA == pFixtureB) return;

        // Make sure only one of the fixtures was a sensor
        bool sensorA = pFixtureA->IsSensor();
        bool sensorB = pFixtureB->IsSensor();
        if (!(sensorA ^ sensorB)) return;

        auto* pColliderA = static_cast<Collider2DComponent*>(pFixtureA->GetBody()->GetUserData());
        auto* pColliderB = static_cast<Collider2DComponent*>(pFixtureB->GetBody()->GetUserData());

        m_contact2Ds.push_back({Contact2D::Type::End,
                               OStaticCast<Collider2DComponent>(pColliderA->shared_from_this()),
                               OStaticCast<Collider2DComponent>(pColliderB->shared_from_this())});
    }
};

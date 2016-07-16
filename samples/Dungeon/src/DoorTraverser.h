#pragma once
#include <onut/Component.h>

#include <onut/ForwardDeclaration.h>
OForwardDeclare(TiledMapComponent);

class DoorTraverser final : public OComponent
{
public:
    void onCreate() override;
    void onTriggerEnter(const OCollider2DComponentRef& pCollider) override;

private:
    OTiledMapComponentRef m_pTiledMapComponent;
};

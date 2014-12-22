#pragma once
#include <cstdint>
#include <cassert>

namespace onut {
    /**
        Pool of abitrary memory. Default allocating ~64k of memory
        template arguments:
        - TobjSize: Size of each objects. Default 256
        - TobjCount: Count of maximum allowed object. Default 256
        - Talignement: Alignement. This is required on some platforms, like iOS. Default sizeof(uintptr_t)
        - TuseAssert: If true, asserts will be used if out of memory or double deletion. Otherwise it will just return nullptr
        - TobjHeaderSize: Default is 1 (Used or unused)
        - TobjTotalSize: Default is TobjSize + TheaderSize size, aligned for Talignement.
        - TmemorySize: Total required memory. Default is TobjTotalSize * TobjCount + Talignment. It accounts for alignment.
    */
    template<
        uintptr_t TobjSize = 256,
        uintptr_t TobjCount = 256,
        uintptr_t Talignment = sizeof(uintptr_t),
        bool TuseAsserts = true,
        uintptr_t TheaderSize = 1,
        uintptr_t TobjTotalSize = ((TobjSize + TheaderSize) % Talignment) ?
            (TobjSize + TheaderSize) + (Talignment - ((TobjSize + TheaderSize) % Talignment)) :
            (TobjSize + TheaderSize),
        uintptr_t TmemorySize = TobjTotalSize * TobjCount + Talignment>
    class Pool {
    public:
        /**
            Constructor. Will allocate the memory
        */
        Pool() {
            // Allocate memory
            m_pMemory = new uint8_t[TmemorySize];
            memset(m_pMemory, 0, TmemorySize);

            // Align
            {
                auto mod = reinterpret_cast<uintptr_t>(m_pMemory) % Talignment;
                if (mod) {
                    m_pFirstObj = m_pMemory + (Talignment - mod);
                }
                else {
                    m_pFirstObj = m_pMemory;
                }
            }
            m_nextFree = m_pFirstObj;
        }

        /**
            Destructor. Will free the memory
        */
        virtual ~Pool() {
            delete[] m_pMemory;
        }

        /**
            Allocate an object.
            @param args Parameter list for your constructor
        */
        template<typename Ttype, typename ... Targs>
        Ttype* alloc(Targs... args) {
            // Loop the pool from the last time.
            auto startPoint = m_nextFree;
            while (true) {
                auto used = m_nextFree + TobjSize;
                if (*used) {
                    m_nextFree += TobjTotalSize;
                    if (m_nextFree > m_pMemory + TmemorySize) {
                        // Wrap
                        m_nextFree = m_pFirstObj;
                    }
                    if (startPoint == m_nextFree) {
                        if (TuseAsserts) {
                            assert(false); // No more memory available in the pool. Use bigger pool
                        }
                        return nullptr;
                    }
                    continue;
                }
                else {
                    // Found one!
                    *used = 1;
                    Ttype* pRet = new(m_nextFree)Ttype(args...);
                    m_nextFree += TobjTotalSize;
                    if (m_nextFree > m_pMemory + TmemorySize) {
                        // Wrap
                        m_nextFree = m_pFirstObj;
                    }
                    return pRet;
                }
            }
        }

        void dealloc(void* pObj) {
            auto ptr = static_cast<uint8_t*>(pObj);
            auto used = ptr + TobjSize;
            if (!*used) {
                if (TuseAsserts) {
                    assert(false); // Memory was already deallocated. Double deletion!
                }
                return;
            }
            *used = 0;
        }

    protected:
        uint8_t*    m_pMemory;
        uint8_t*    m_pFirstObj;
        uint8_t*    m_nextFree;
    };
}

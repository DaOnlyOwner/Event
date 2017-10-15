//
// Created by Jan_M720 on 14.10.2017.
//

#pragma once

#include <vector>
#include <memory>
#include "Delegate.h"
#include <algorithm>

namespace doo
{

    template <typename... TArgs>
    class Event
    {
    public:
        typedef Delegate<void (TArgs...)> DelegateType;

        DelegateID Subscribe(DelegateType type)
        {
            delegate_helper helper;
            helper.delegate = std::move(type);
            helper.id = m_id_counter++;
            m_delegates.push_back(helper);
        }

        bool Unsubscribe(DelegateID id)
        {
            auto it = std::find_if(m_delegates.begin(), m_delegates.end(), [id](const delegate_helper& helper){
                return helper.id == id;});
            if (it == m_delegates.end())
                return false;
            std::swap(*it,m_delegates.back());
            m_delegates.pop_back();
            return true;
        }

        void operator() (TArgs... args)
        {
            for(delegate_helper& helper : m_delegates)
            {
                helper.delegate(args...);
            }
        }

    private:
        struct delegate_helper
        {
            DelegateType delegate;
            DelegateID id;
        };

    private:
        std::vector<delegate_helper> m_delegates;
        DelegateID m_id_counter = 0;
    };

}

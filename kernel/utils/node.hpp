#pragma once
#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "../common.hpp"

namespace utl
{
       template <typename data_node_type>
       class RawNode;

       template <typename data_node_type>
       RawNode<data_node_type> *new_node();

       template <typename data_node_type>
       RawNode<data_node_type> *new_node(const data_node_type &data);

       template <typename data_node_type>
       void delete_node(RawNode<data_node_type> *node);

       template <typename data_node_type>
       class RawNode
       {
       protected:
              data_node_type m_data;
              RawNode<data_node_type> *m_previous, *m_next;
              RawNode() : m_previous(nullptr), m_next(nullptr)
              {
              }
              RawNode(const data_node_type &data) : m_previous(nullptr), m_next(nullptr), m_data(data)
              {
              }
              ~RawNode()
              {
                     disconnect();
              }

       public:
              friend RawNode *new_node<>();
              friend RawNode *new_node<>(const data_node_type &data);
              friend void delete_node<>(RawNode *node);
              void connect_next(RawNode *node)
              {
                     if (!is_back())
                     {
                            m_next->m_previous = node;
                            node->m_next = m_next;
                     }
                     m_next = node;
                     node->m_previous = this;
              }
              void connect_previous(RawNode *node)
              {
                     if (!is_front())
                     {
                            m_previous->m_next = node;
                            node->m_previous = m_previous;
                     }
                     m_previous = node;
                     node->m_next = this;
              }
              void disconnect()
              {
                     if (!is_front() && !is_back())
                     {
                            m_next->m_previous = m_previous;
                            m_previous->m_next = m_next;
                     }
                     else if (!is_front())
                     {
                            m_previous->m_next = nullptr;
                     }
                     else if (!is_back())
                     {
                            m_next->m_previous = nullptr;
                     }
                     m_next = nullptr;
                     m_previous = nullptr;
              }
              inline data_node_type &data() noexcept
              {
                     return m_data;
              }
              inline const data_node_type &data() const noexcept
              {
                     return m_data;
              }
              inline bool is_front() const noexcept
              {
                     return m_previous == nullptr;
              }
              inline bool is_back() const noexcept
              {
                     return m_next == nullptr;
              }
              inline RawNode *previous() noexcept
              {
                     return m_previous;
              }
              inline RawNode *next() noexcept
              {
                     return m_next;
              }
              inline const RawNode *previous() const noexcept
              {
                     return m_previous;
              }
              inline const RawNode *next() const noexcept
              {
                     return m_next;
              }
       };

       template <typename data_node_type>
       using node = RawNode<data_node_type> *;

       constexpr void *nullnode = nullptr;

       template <typename data_node_type>
       RawNode<data_node_type> *new_node()
       {
              return new RawNode<data_node_type>();
       }

       template <typename data_node_type>
       RawNode<data_node_type> *new_node(const data_node_type &data)
       {
              return new RawNode<data_node_type>(data);
       }

       template <typename data_node_type>
       void delete_node(RawNode<data_node_type> *node)
       {
              delete node;
       }

       template<typename data_node_type>
       RawNode<data_node_type> *create_node_list_next(size_t count)
       {
              if(count == 0)
              {
                     return nullptr;
              }
              RawNode<data_node_type> *curr_node = new_node<data_node_type>(), *head = curr_node;
              while(--count)
              {
                     RawNode<data_node_type> *next_node = new_node<data_node_type>();
                     curr_node->connect_next(next_node);
                     curr_node = next_node;
              }
              return head;
       }

       template<typename data_node_type>
       RawNode<data_node_type> *create_node_list_next(size_t count, const data_node_type *data)
       {
              if(count == 0)
              {
                     return nullptr;
              }
              RawNode<data_node_type> *curr_node = new_node<data_node_type>(data[0]), *head = curr_node;
              for(size_t i = 1; i < count; ++i)
              {
                     RawNode<data_node_type> *next_node = new_node<data_node_type>(data[i]);
                     curr_node->connect_next(next_node);
                     curr_node = next_node;
              }
              return head;
       }

       template<typename data_node_type>
       void delete_all_next_nodes(RawNode<data_node_type> *node)
       {
              RawNode<data_node_type> *curr_node = node->next();
              while(curr_node != nullnode)
              {
                     RawNode<data_node_type> *next_node = curr_node->next();
                     delete_node(curr_node);
                     curr_node = next_node;
              }
       }
}

#endif //__NODE_HPP__
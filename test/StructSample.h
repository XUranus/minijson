/*================================================================
*   Copyright (C) 2022 XUranus All rights reserved.
*   
*   File:         StructSample.h
*   Author:       XUranus
*   Date:         2022-11-21
*   Description:  a tiny C++ Json library
*                 https://github.com/XUranus/minicpp
*
================================================================*/

#include <string>
#include <utility>
#include <vector>
#include <list>
#include <map>
#include "../Json.h"

struct Book {
    std::string                 m_name;
    int                         m_id;
    float                       m_currentPrice;
    bool                        m_soldOut;
    std::vector<std::string>    m_tags;
    std::pair<float, float>     m_priceRange;

    SERIALIZE_SECTION_BEGIN
    SERIALIZE_FIELD(name, m_name);
    SERIALIZE_FIELD(id, m_id);
    SERIALIZE_FIELD(price, m_currentPrice);
    SERIALIZE_FIELD(soldOut, m_soldOut);
    SERIALIZE_FIELD(tags, m_tags);
    SERIALIZE_FIELD(priceRange, m_priceRange);
    SERIALIZE_SECTION_END
};

struct Author {
    std::string                 m_name;
    std::list<Book>             m_books;
    
    SERIALIZE_SECTION_BEGIN
    SERIALIZE_FIELD(name, m_name);
    SERIALIZE_FIELD(books, m_books);
    SERIALIZE_SECTION_END
};

bool operator == (const Book& book1, const Book& book2)
{
    return (
        book1.m_name == book2.m_name &&
        book1.m_id == book2.m_id &&
        book1.m_currentPrice == book2.m_currentPrice &&
        book1.m_soldOut == book2.m_soldOut &&
        book1.m_tags == book2.m_tags
    );
}
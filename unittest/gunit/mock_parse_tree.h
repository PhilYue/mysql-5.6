#ifndef MOCK_PARSE_TREE_INCLUDED
#define MOCK_PARSE_TREE_INCLUDED
/* Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

/**
  @file mock_parse_tree.h

  Mock classes for parse tree nodes.
*/

#include "sql/parse_tree_items.h"

namespace my_testing {

inline void fix(THD *thd, std::initializer_list<Item *> items) {
  Parse_context pc(thd, thd->lex->select_lex);
  for (Item *item : items) {
    item->itemize(&pc, &item);
    item->fix_fields(thd, nullptr);
  }
}

class Mock_text_literal : public PTI_text_literal_text_string {
 public:
  Mock_text_literal(const char *s)
      : PTI_text_literal_text_string(POS(), false,
                                     {const_cast<char *>(s), strlen(s)}) {}
};

inline Item *make_fixed_literal(THD *thd, const char *pattern) {
  if (pattern == nullptr) return new Item_null;
  auto item = new Mock_text_literal(pattern);
  fix(thd, {item});
  return item;
}

class Mock_pt_item_list : public PT_item_list {
 public:
  Mock_pt_item_list(std::initializer_list<const char *> strings) {
    for (auto string : strings) push_back(new Mock_text_literal(string));
  }
};

}  // namespace my_testing

#endif  // MOCK_PARSE_TREE_INCLUDED

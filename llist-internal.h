/* Copyright © 2021-2023 Chee Bin HOH. All rights reserved.
 *
 * A linked list (single).
 */

#ifndef LLIST_INTERNAL_H_HAS_INCLUDED

#define LLIST_INTERNAL_H_HAS_INCLUDED

union ListNodeData {
  void *ref;
  int val;
};

struct ListNode {

  union ListNodeData data;
  struct ListNode *next;
};

#endif

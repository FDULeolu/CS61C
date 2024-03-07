#include <stddef.h>
#include "ll_cycle.h"

int ll_has_cycle(node *head) {
    if (head == NULL) {
        return 0;
    }
    
    node* fast;
    node* slow;
    fast = head;
    slow = head;
    do {
        if (fast->next == NULL || fast->next->next == NULL) {
            return 0;
        }
        fast = fast->next->next;
        slow = slow->next;
        if (fast == NULL) {
            return 0;
        }
    } while (fast != slow);
    return 1;
}
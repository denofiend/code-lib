/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
 /*
 *   convert single-linked list to array, O(1) to get one element.
 *   array = [1, 2, 3, 4, 5, 6];
 *   array[left] = 1;
 *   array[rigth] = 6;
 *   root = array[mid];
 *   left = array[left, mid-1];
 *   right = array[mid+1, right];
 */
class Solution {
public:
    TreeNode *sortedListToBST(ListNode *head) {
        ListNode *iter = head;
        int n = 0;
        while (NULL != iter) {
            n++;
            iter = iter->next;
        }

        int i = 0;
        ListNode **array = new ListNode* [n+1];
        iter = head;
        while (NULL != iter) {
            array[i] = iter;
            iter = iter->next;
            i++;
        }

        return sortedListToBSTV1(array, 0, n-1);
    }

    ListNode* getNthNode(ListNode** head, int n) {
        return head[n];
    }
    TreeNode *sortedListToBSTV1(ListNode** head, int sta, int end) {
        if (sta > end || end < sta) {
            return NULL;
        }
        else if (sta == end) {
            ListNode *ln = getNthNode(head, sta);
            TreeNode *tn = new TreeNode(ln->val);
            return tn;
        }

        int mid = (sta + end + 1) / 2;

        ListNode *ln = getNthNode(head, mid);
        TreeNode *tn = new TreeNode(ln->val);

        tn->left = sortedListToBSTV1(head, sta, mid-1);
        tn->right = sortedListToBSTV1(head, mid+1, end);

        return tn;
    }
};
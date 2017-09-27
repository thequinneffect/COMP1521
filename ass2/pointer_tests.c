#include <stdio.h>
#include <stdlib.h>

struct number {
	int val;
	struct number *next;
	struct number *prev;
};

struct number **num_getter;

int main() {

	int amount = 3;
	struct number *set;
	set = malloc(amount * sizeof(struct number));
	num_getter = malloc(sizeof(int*)*amount);

	for (int i = 0; i < amount; i++) {
    	struct number *num = &set[i];
      	num_getter[i] = num;
    }

    struct number *example = num_getter[0];
    printf("%d\n", example->val);

	struct number *head = &set[0];
	struct number *second = &set[1];
	struct number *tail = &set[2];
	head->val = 0;
	second->val = 1;
	tail->val = 2;
	head->prev = NULL;
	head->next = second;
	second->prev = head;
	second->next = tail;
	tail->prev = second;
	tail->next = NULL;

	struct number *curr = head;
	printf("ordered list: ");
	while (curr != NULL)
	{
		printf("%d ", curr->val);
		curr = curr->next;
	}
	printf("\n");

	printf("num getter 0: %d\n", num_getter[0]->val);
	printf("num getter 1: %d\n", num_getter[1]->val);
	printf("num getter 2: %d\n", num_getter[2]->val);

	struct number *temp = head;
	head = tail;
	tail = temp;
	head->prev = NULL;
	head->next = second;
	second->prev = head;
	second->next = tail;
	tail->prev = second;
	tail->next = NULL;

	curr = head;
	printf("ordered list: ");
	while (curr != NULL)
	{
		printf("%d ", curr->val);
		curr = curr->next;
	}
	printf("\n");

	printf("num getter 0: %d\n", num_getter[0]->val);
	printf("num getter 1: %d\n", num_getter[1]->val);
	printf("num getter 2: %d\n", num_getter[2]->val);

	return 0;
}
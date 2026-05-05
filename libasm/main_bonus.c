#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>


typedef struct s_list {
    void *data;
    struct s_list *next;
} t_list;

t_list *new_node(void *data)
{
    t_list *n = malloc(sizeof(t_list));
    n->data = data;
    n->next = NULL;
    return n;
}

int ft_list_size(t_list *begin_list);
void ft_list_push_front(t_list **begin_list, void *data);
void ft_list_remove_if(t_list **begin_list, void *data_ref,
                       int (*cmp)(), void (*free_fct)(void *));
void ft_list_sort(t_list **begin_list, int (*strcmp)());
int ft_atoi_base(char *str, char *base);



void print_list(t_list *lst)
{
    while (lst)
    {
        printf("%s -> ", (char *)lst->data);
        lst = lst->next;
    }
    printf("NULL\n");
}

void ft_list_clear(t_list **list)
{
  t_list *tmp;

  while (*list)
  {
    tmp = (*list)->next;
    if ((*list)->data)
        free((*list)->data);
    free(*list);
    *list = tmp;
  }
}

void test_empty()
{
    t_list *list = NULL;

    ft_list_sort(&list, strcmp);

    printf("Empty: ");
    print_list(list); // attendu: NULL
}

void test_one()
{
    t_list *list = NULL;
    ft_list_push_front(&list, "hello");

    ft_list_sort(&list, strcmp);

    printf("One: ");
    print_list(list); // hello
}

void test_sorted()
{
    t_list *list = NULL;

    ft_list_push_front(&list, "c");
    ft_list_push_front(&list, "b");
    ft_list_push_front(&list, "a");

    ft_list_sort(&list, strcmp);

    printf("Sorted: ");
    print_list(list); // a b c
}

void test_random()
{
    t_list *list = NULL;

    ft_list_push_front(&list, "b");
    ft_list_push_front(&list, "a");
    ft_list_push_front(&list, "c");

    ft_list_sort(&list, strcmp);

    printf("Random: ");
    print_list(list); // a b c
}

void test_duplicates()
{
    t_list *list = NULL;

    ft_list_push_front(&list, "b");
    ft_list_push_front(&list, "a");
    ft_list_push_front(&list, "b");
    ft_list_push_front(&list, "a");

    ft_list_sort(&list, strcmp);

    printf("Duplicates: ");
    print_list(list); // a a b b
}

void test_reverse()
{
    t_list *list = NULL;

    ft_list_push_front(&list, "a");
    ft_list_push_front(&list, "b");
    ft_list_push_front(&list, "c");
    ft_list_push_front(&list, "d");

    ft_list_sort(&list, strcmp);

    printf("Reverse: ");
    print_list(list); // a b c d
}

void test_strings()
{
    t_list *list = NULL;

    ft_list_push_front(&list, "zzz");
    ft_list_push_front(&list, "abc");
    ft_list_push_front(&list, "Abc");
    ft_list_push_front(&list, "aaa");

    ft_list_sort(&list, strcmp);

    printf("Strings: ");
    print_list(list);
}

int main(void)
{
    t_list *list = NULL;

    printf("\n=== list_push_front ===\n");
    ft_list_push_front(&list, strdup("world"));
    ft_list_push_front(&list, strdup("hello"));
    ft_list_push_front(&list, strdup("foo"));
    ft_list_push_front(&list, strdup("bar"));

    printf("List after push_front:\n");
    print_list(list);

    printf("\n=== list_size ===\n");
    printf("Size: %d\n", ft_list_size(list));

    printf("\n=== list_remove_if ===\n");
    printf("\nRemoving 'foo'...\n");
    ft_list_remove_if(&list, "foo", &strcmp, &free);

    print_list(list);
    printf("\n=== list_size ===\n");
    printf("Size: %d\n", ft_list_size(list));

    printf("\n=== list_remove_if ===\n");
    printf("\nRemoving 'bar'...\n");
    ft_list_remove_if(&list, "bar", &strcmp, &free);

    print_list(list);
    printf("\n=== list_size ===\n");
    printf("Size: %d\n", ft_list_size(list));

    printf("\n=== list_remove_if ===\n");
    printf("\nRemoving 'hello'...\n");
    ft_list_remove_if(&list, "hello", &strcmp, &free);

    print_list(list);
    printf("\n=== list_size ===\n");
    printf("Size: %d\n", ft_list_size(list));

    printf("\n=== list_remove_if ===\n");
    printf("\nRemoving 'world'...\n");
    ft_list_remove_if(&list, "world", &strcmp, &free);

    print_list(list);
    printf("\n=== list_size ===\n");
    printf("Size: %d\n", ft_list_size(list));

    ft_list_clear(&list);

    printf("\n=== list_sort ===\n");
    test_empty();
    test_one();
    test_sorted();
    test_random();
    test_duplicates();
    test_reverse();
    test_strings();


    printf("\n=== atoi_base ===\n");
    printf("%d\n", ft_atoi_base("101", "01")); // 5
    printf("%d\n", ft_atoi_base("-1A", "0123456789ABCDEF")); // -26
    printf("%d\n", ft_atoi_base("   +42", "0123456789")); // 42
    printf("%d\n", ft_atoi_base("zzz", "z")); // 0 (invalid base)
    printf("39: %d\n", ft_atoi_base("1", "01\v")); // expected: 0
    printf("40: %d\n", ft_atoi_base("1", "01\f")); // expected: 0
    printf("41: %d\n", ft_atoi_base("1", "01\r")); // expected: 0
    printf("42: %d\n", ft_atoi_base("", "01"));     // expected: 0

    return 0;
}

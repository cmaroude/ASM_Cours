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
    printf("bin 101 (01) = %d (expected 5)\n",
    ft_atoi_base("101", "01"));

    printf("bin 1111 (01) = %d (expected 15)\n",
    ft_atoi_base("1111", "01"));

    printf("bin -100 (01) = %d (expected -4)\n",
    ft_atoi_base("-100", "01"));

    printf("dec 42 (0123456789) = %d (expected 42)\n",
    ft_atoi_base("42", "0123456789"));

    printf("dec -42 = %d (expected -42)\n",
    ft_atoi_base("-42", "0123456789"));

    printf("dec +123 = %d (expected 123)\n",
    ft_atoi_base("   +123", "0123456789"));

    printf("hex 1A = %d (expected 26)\n",
    ft_atoi_base("1A", "0123456789ABCDEF"));

    printf("hex -FF = %d (expected -255)\n",
    ft_atoi_base("-FF", "0123456789ABCDEF"));

    printf("hex 10 = %d (expected 16)\n",
    ft_atoi_base("10", "0123456789ABCDEF"));

    printf("abc abc = %d (expected 5)\n",
    ft_atoi_base("abc", "abc"));

    printf("abc cab = %d (expected 15)\n",
    ft_atoi_base("cab", "abc"));
    
    printf("ba ba = %d (expected 2)\n",
    ft_atoi_base("ba", "ba"));

    printf("abba (ab) = %d (expected 6)\n",
    ft_atoi_base("abba", "ab"));

    printf("empty = %d (expected 0)\n",
    ft_atoi_base("", "0123456789"));

    printf("invalid base len1 = %d (expected 0)\n",
    ft_atoi_base("42", "0"));

    printf("invalid +- = %d (expected 0)\n",
    ft_atoi_base("42", "+-01"));

    printf("whitespace base = %d (expected 0)\n",
    ft_atoi_base("42", "01\v"));

    printf("stop parsing 42abc = %d (expected 42)\n",
    ft_atoi_base("42abc", "0123456789"));
    
    return 0;
}

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct s_list {
    void *data;
    struct s_list *next;
} t_list;

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

int ft_list_size(t_list *begin_list);
void ft_list_push_front(t_list **begin_list, void *data);
void ft_list_remove_if(t_list **begin_list, void *data_ref,
                       int (*cmp)(), void (*free_fct)(void *));


int main(void)
{
    t_list *list = NULL;

    // ===== PUSH FRONT =====
    ft_list_push_front(&list, strdup("world"));
    ft_list_push_front(&list, strdup("hello"));
    ft_list_push_front(&list, strdup("foo"));
    ft_list_push_front(&list, strdup("bar"));

    printf("List after push_front:\n");
    print_list(list);

    // ===== SIZE =====
    printf("Size: %d\n", ft_list_size(list));

    // ===== REMOVE IF =====
    printf("\nRemoving 'foo'...\n");
    ft_list_remove_if(&list, "foo", &strcmp, &free);

    print_list(list);
    printf("Size: %d\n", ft_list_size(list));

    printf("\nRemoving 'bar'...\n");
    ft_list_remove_if(&list, "bar", &strcmp, &free);

    print_list(list);
    printf("Size: %d\n", ft_list_size(list));

    printf("\nRemoving 'hello'...\n");
    ft_list_remove_if(&list, "hello", &strcmp, &free);

    print_list(list);
    printf("Size: %d\n", ft_list_size(list));

    printf("\nRemoving 'world'...\n");
    ft_list_remove_if(&list, "world", &strcmp, &free);

    print_list(list);
    printf("Size: %d\n", ft_list_size(list));

    return 0;
}

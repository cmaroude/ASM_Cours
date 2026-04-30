int ft_list_size(t_list *begin_list);
void ft_list_push_front(t_list **begin_list, void *data);
void ft_list_remove_if(t_list **begin_list, void *data_ref,
                       int (*cmp)(), void (*free_fct)(void *));

void print_list(t_list *lst)
{
    while (lst)
    {
        printf("%s -> ", (char *)lst->data);
        lst = lst->next;
    }
    printf("NULL\n");
}

int main(void)
{
    t_list *list = NULL;

    // ===== PUSH FRONT =====
    ft_list_push_front(&list, "world");
    ft_list_push_front(&list, "hello");
    ft_list_push_front(&list, "foo");
    ft_list_push_front(&list, "bar");

    printf("List after push_front:\n");
    print_list(list);

    // ===== SIZE =====
    printf("Size: %d\n", ft_list_size(list));

    // ===== REMOVE IF =====
    printf("\nRemoving 'foo'...\n");
    ft_list_remove_if(&list, "foo", cmp, free_fct);

    print_list(list);
    printf("Size: %d\n", ft_list_size(list));

    printf("\nRemoving 'bar'...\n");
    ft_list_remove_if(&list, "bar", cmp, free_fct);

    print_list(list);
    printf("Size: %d\n", ft_list_size(list));

    printf("\nRemoving 'hello'...\n");
    ft_list_remove_if(&list, "hello", cmp, free_fct);

    print_list(list);
    printf("Size: %d\n", ft_list_size(list));

    printf("\nRemoving 'world'...\n");
    ft_list_remove_if(&list, "world", cmp, free_fct);

    print_list(list);
    printf("Size: %d\n", ft_list_size(list));

    return 0;
}

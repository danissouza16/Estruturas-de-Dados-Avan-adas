// Daniel Santana Souza - 2310995
// Laboratório 4

#include <stdio.h>

typedef unsigned char Permissions;

static const char *nomes[8] = {
    "leitura",
    "escrita",
    "execução",
    "apagar",
    "criar_pastas",
    "compartilhar",
    "administrar",
    "configurações"
};

void grantPermission(Permissions *p, int bit) {
    if (bit < 0 || bit > 7) return;
    *p |= (1 << bit);
}

void revokePermission(Permissions *p, int bit) {
    if (bit < 0 || bit > 7) return;
    *p &= ~(1 << bit);
}

int hasPermission(Permissions p, int bit) {
    if (bit < 0 || bit > 7) return 0;
    return (p & (1 << bit)) != 0;
}

void showPermissions(Permissions p) {
    printf("{ ");
    for (int i = 0; i < 8; i++) {
        if (hasPermission(p, i)) {
            printf("%s ", nomes[i]);
        }
    }
    printf("}\n");
}

int main(void) {
    Permissions user1 = 0;
    Permissions user2 = 0;

    grantPermission(&user1, 1); // escrita
    grantPermission(&user1, 4); // criar pastas
    grantPermission(&user1, 5); // compartilhar arquivos
    grantPermission(&user1, 7); // alterar configurações

    grantPermission(&user2, 2); // execução
    grantPermission(&user2, 4); // criar pastas
    grantPermission(&user2, 5); // compartilhar arquivos
    grantPermission(&user2, 6); // administrar usuários

    printf("Permissões do Usuário 1:\n");
    showPermissions(user1);

    printf("\nPermissões do Usuário 2:\n");
    showPermissions(user2);

    Permissions unionPerm = user1 | user2;
    printf("\nPermissões combinadas (união):\n");
    showPermissions(unionPerm);

    Permissions interPerm = user1 & user2;
    printf("\nPermissões em comum (interseção):\n");
    showPermissions(interPerm);

    printf("\nUsuário 1 pode apagar arquivos? %s\n",
           hasPermission(user1, 3) ? "Sim" : "Não");
    printf("Usuário 2 pode administrar usuários? %s\n",
           hasPermission(user2, 6) ? "Sim" : "Não");
    printf("Usuário 1 pode criar pastas? %s\n",
           hasPermission(user1, 4) ? "Sim" : "Não");

    printf("\nRevogando permissão de compartilhar do Usuário 1...\n");
    revokePermission(&user1, 5);
    printf("Permissões do Usuário 1 após revogação:\n");
    showPermissions(user1);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX

typedef struct Edge {
    int v;
    int w;
    struct Edge *next;
} Edge;

typedef struct Vertex {
    Edge *firstEdge;
} Vertex;

void destroy_graph(Vertex *graph, int size) {
    while(--size >= 0) {
        Edge *e = graph[size].firstEdge;
        while (e != NULL) {
            Edge *temp = e->next;
            free(e);
            e = temp;
        }
    }
    free(graph);
}

void print_graph(Vertex *graph, int size) {
    int i;
    for (i = 0; i < size; i++) {
        Edge *e = graph[i].firstEdge;
        while (e != NULL) {
            printf("%d == %d ==> %d\n", i+1, e->w, e->v+1);
            e = e->next;
        }
    }
}

int main(int argc, const char *argv[])
{
    int num_places = 0, num_branches = 0, num_connections = 0;
    int i, j;
    int *branches = NULL;
    Vertex *places = NULL;
    int *weights = NULL;

    scanf("%d %d %d", &num_places, &num_branches, &num_connections);
    branches = malloc(sizeof(*branches)*num_branches);
    places = calloc(num_places, sizeof(*places));
    weights = malloc(sizeof(int)*num_places*num_places);

    for (i = 0; i < num_places; i++) {
        for (j = 0; j < num_places; j++) {
            weights[i + j*num_places] = (i == j) ? 0 : INF;
        }
    }

    for (i = 0; i < num_branches; i++) {
        scanf("%d", branches+i);
        branches[i]--;
    }

    for (i = 0; i < num_connections; i++) {
        int u, v, w;
        scanf("%d", &u);
        Edge *new_connection = malloc(sizeof(Edge));
        scanf("%d %d", &v, &w);
        /* Input starts at 1 so we offset it by -1 internally */
        /* for easier handling of the graph */ 
        --u;
        --v;
        new_connection->v = v;
        new_connection->w = w;
        new_connection->next = places[u].firstEdge;
        places[u].firstEdge = new_connection;
    }

    print_graph(places, num_places);

    destroy_graph(places, num_places);
    free(weights);
    free(branches);

    return 0;
}

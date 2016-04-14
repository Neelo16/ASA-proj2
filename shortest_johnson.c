#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN(A, B) ((A < B) ? A : B)
#define MAX(A, B) ((A > B) ? A : B)
#define swap(A, B) {__typeof__(A) t = A; A = B; B = t;}

typedef struct edge {
    int u;
    int v;
    int w;
    struct edge *next;
} Edge;

typedef struct vertex {
    int heap_index; /* For added efficiency in Dijkstra */
    Edge *first_edge;
} Vertex;

typedef struct heap_node {
    Vertex *vertex;
    int dist;
} Heap_node;

int INF = 0x1337; /* FIXME */

/* Heap functions */

#define heap_parent(i) ((i-1)/2)
#define heap_left(i) (2*i+1)
#define heap_right(i) (2*i+2)
#define heap_key(i) (i.dist)
#define heap_set_key(i, key) i.dist = key;
#define heap_swap(heap, i, j) {heap[i].vertex->heap_index = j; heap[j].vertex->heap_index = i; swap(heap[i], heap[j]);}

void min_heapify(Heap_node heap[], int heap_size, int i) {
    int l = heap_left(i);
    int r = heap_right(i);
    int smallest = i;

    if (l < heap_size && heap_key(heap[l]) < heap_key(heap[i]))
        smallest = l;

    if (r < heap_size && heap_key(heap[r]) < heap_key(heap[smallest]))
        smallest = r;

    if (i != smallest) {
        heap_swap(heap, i, smallest);
        min_heapify(heap, heap_size, smallest);
    }
}

Vertex *heap_extract_min(Heap_node heap[], int heap_size) {
    Vertex *min = heap[0].vertex;
    --heap_size;
    heap_swap(heap, 0, heap_size);
    min_heapify(heap, heap_size, 0);
    return min;
}

void heap_decrease_key(Heap_node heap[], int heap_size, int i, int key) {
    heap_set_key(heap[i], key);
    while (i > 0 && heap_key(heap[heap_parent(i)]) > heap_key(heap[i])) {
        heap_swap(heap, i, heap_parent(i));
        i = heap_parent(i);
    }
}

void build_min_heap(Heap_node heap[], int heap_size) {
    int i = heap_size/2;
    while (i > 0)
        min_heapify(heap, heap_size, i--);
}

void print_heap(Heap_node heap[], int heap_size) {
    int i;
    for (i = 0; i < heap_size; i++)
        printf("%d ", heap_key(heap[i]));
    putchar('\n');
}

/* End heap functions */

void dijkstra(Vertex graph[], int graph_size, int source, int distances[]) {
    Heap_node *heap = malloc(sizeof(Heap_node)*graph_size);
    int i;
    for (i = 0; i < graph_size; i++) {
        heap[i].dist = i == source ? 0 : INF;
        distances[i] = i == source ? 0 : INF;
        heap[i].vertex = graph + i;
        graph[i].heap_index = i;
    }

    heap_swap(heap, 0, source);
    distances[source] = 0;
    
    while (graph_size) {
        Vertex *vertex = heap_extract_min(heap, graph_size--);
        Edge *e = vertex->first_edge;
        while (e != NULL) {
            int u = e->u;
            int v = e->v;
            int w = e->w;
            int new_distance = distances[u] + w;
            if (distances[v] > new_distance) {
                distances[v] = new_distance;
                heap_decrease_key(heap, graph_size,
                                  graph[v].heap_index, new_distance);
            }
            e = e->next;
        }
    }

    free(heap);
}

int *get_reweight_values(Vertex graph[], int graph_size, Edge edges[], int num_edges) {
    int *distances = calloc(graph_size, sizeof(int));
    int i;

    for (i = 0; i < graph_size - 1; i++) {
        int j;
        bool changed = false;

        for (j = 0; j < num_edges; j++) {
            Edge *e = edges + j;
            int u = e->u;
            int v = e->v;
            int w = e->w;
            int new_distance = distances[u] + w;
            if (distances[v] > new_distance) {
                changed = true;
                distances[v] = new_distance;
            }
        }

        if (!changed)
            break;
    }

    return distances;
}

int *get_shortest_paths_matrix(Vertex graph[], int graph_size,
                               Edge edges[], int num_edges,
                               int branches[], int num_branches) {
    int *distances = malloc(sizeof(int)*graph_size*num_branches);
    int *h = get_reweight_values(graph, graph_size, edges, num_edges);
    int i;


    for (i = 0; i < num_edges; i++) {
        Edge *e = edges + i;
        int u = e->u;
        int v = e->v;
        int w = e->w;
        e->w = w + h[u] - h[v];
    }
    
    for (i = 0; i < num_branches; i++) {
        int j;
        dijkstra(graph, graph_size, branches[i], distances + i*graph_size);
        for (j = 0; j < graph_size; j++) {
            int reweighted = distances[i*graph_size + j];
            if (reweighted == INF)
                continue;
            distances[i*graph_size + j] = reweighted + h[j] - h[branches[i]];
        }
    }

    free(h);
    return distances;
}

void print_graph(Vertex *graph, int size) {
    int i;
    for (i = 0; i < size; i++) {
        Edge *e = graph[i].first_edge;
        while (e != NULL) {
            printf("%d == %d ==> %d\n", i+1, e->w, e->v+1);
            e = e->next;
        }
    }
}

void print_matrix(int *matrix, int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (matrix[i*size + j] == INF)
                printf("%s", "| INF ");
            else
                printf("| %d ", matrix[i*size + j]);
        }
        puts("|");
    }
}

int main(int argc, const char *argv[])
{
    int num_places = 0, num_branches = 0, num_connections = 0;
    int i;
    int *branches = NULL;
    Vertex *places = NULL;
    Edge *connections = NULL;
    int *paths = NULL;

    scanf("%d %d %d", &num_places, &num_branches, &num_connections);
    branches = malloc(sizeof(*branches)*num_branches);
    places = calloc(num_places, sizeof(*places));
    connections = malloc(sizeof(Edge)*num_connections);

    for (i = 0; i < num_branches; i++) {
        scanf("%d", branches+i);
        /* Input starts at 1 so we offset it by -1 internally */
        /* for easier handling of the graph */ 
        branches[i]--;
    }

    {
        bool max_defined = false;
        int max_weight = 0;

        for (i = 0; i < num_connections; i++) {
            int u, v, w;
            scanf("%d", &u);
            Edge *new_connection = connections + i;
            scanf("%d %d", &v, &w);
            /* We decrement same reason as given above for branches */
            --u;
            --v;
            new_connection->u = u;
            new_connection->v = v;
            new_connection->w = w;
            new_connection->next = places[u].first_edge;
            places[u].first_edge = new_connection;
            if (max_defined)
                max_weight = MAX(max_weight, w);
            else {
                max_defined = true;
                max_weight = w;
            }
        }

        INF = abs(num_places*max_weight) + 1;
    }

    {
        int i, j;
        int total_loss = INF;
        int meeting_place = 0;
        bool reachable = false;
        paths = get_shortest_paths_matrix(places, num_places,
                                          connections, num_connections,
                                          branches, num_branches);

        for (i = 0; i < num_places; i++) {
            int current_loss = 0;
            reachable = true;
            for (j = 0; j < num_branches; j++) {
                int weight = paths[j*num_places + i];
                if (weight == INF) {
                    reachable = false;
                    break;
                }
                current_loss += weight;
            }
            if (reachable && current_loss < total_loss) {
                total_loss = current_loss;
                meeting_place = i;
            }
        }

        if (!reachable)
            puts("N");
        else {
            printf("%d %d\n", meeting_place+1, total_loss);
            for (i = 0; i < num_branches; i++) {
                printf("%d ", paths[i*num_places + meeting_place]);
            }
            putchar('\n');
        }
    }   

    free(branches);
    free(places);
    free(connections);
    free(paths);

    return 0;
}
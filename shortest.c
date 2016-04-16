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

typedef struct result {
    int meeting_place;
    int total_loss;
    int *distances;
} Result;

static int INF;

/* Heap functions */

#define heap_parent(i) ((i-1)/2)
#define heap_left(i) (2*i+1)
#define heap_right(i) (2*i+2)

void heap_swap(int heap[], int i, int j, Vertex graph[]) {
    int u = heap[i];
    int v = heap[j];
    swap(heap[i], heap[j]);
    swap(graph[u].heap_index, graph[v].heap_index);
}

void min_heapify(int heap[], size_t heap_size, int i, Vertex graph[], int keys[]) {
    int l = heap_left(i);
    int r = heap_right(i);
    int smallest = i;

    if (l < heap_size && keys[heap[l]] < keys[heap[i]])
        smallest = l;

    if (r < heap_size && keys[heap[r]] < keys[heap[smallest]])
        smallest = r;

    if (i != smallest) {
        heap_swap(heap, i, smallest, graph);
        min_heapify(heap, heap_size, smallest, graph, keys);
    }
}

int heap_extract_min(int heap[], size_t heap_size, Vertex graph[], int keys[]) {
    int min = heap[0];
    heap_swap(heap, 0, --heap_size, graph);
    min_heapify(heap, heap_size, 0, graph, keys);
    return min;
}

void heap_decrease_key(int heap[], size_t heap_size, int i, int key, Vertex graph[], int keys[]) {
    keys[heap[i]] = key;
    while (i > 0 && keys[heap[heap_parent(i)]] > keys[heap[i]]) {
        heap_swap(heap, i, heap_parent(i), graph);
        i = heap_parent(i);
    }
}

/* End heap functions */

void transpose_graph(Vertex graph[], size_t graph_size,
                     Edge   edges[], size_t num_edges) {
    int i;
    for (i = 0; i < graph_size; i++)
        graph[i].first_edge = NULL;
    for (i = 0; i < num_edges; i++) {
        Edge *edge = edges + i;
        int u = edge->u;
        int v = edge->v;
        edge->u = v;
        edge->v = u;
        edge->next = graph[v].first_edge;
        graph[v].first_edge = edge;
    }
}

void dijkstra(Vertex graph[], int graph_size, int source, int result[]) {
    /* Modifies result and stores the solution there */
    int *heap = malloc(sizeof(int)*graph_size);
    int i;
    for (i = 0; i < graph_size; i++) {
        result[i] = i == source ? 0 : INF;
        heap[i] = graph[i].heap_index = i;
    }

    heap_swap(heap, 0, source, graph);
    
    while (graph_size) {
        int u = heap_extract_min(heap, graph_size--, graph, result);
        Edge *e = graph[u].first_edge;
        while (e != NULL) {
            int v = e->v;
            int w = e->w;
            int new_distance = result[u] + w;
            if (result[v] > new_distance) {
                heap_decrease_key(heap, graph_size,
                                  graph[v].heap_index, new_distance,
                                  graph, result);
            }
            e = e->next;
        }
    }
    free(heap);
}

int *get_reweight_values(Vertex graph[], int graph_size) {
    int *distances = calloc(graph_size, sizeof(int));
    int i;

    for (i = 0; i < graph_size - 1; i++) {
        int u;
        bool changed = false;

        for (u = 0; u < graph_size; u++) {
            Edge *e = graph[u].first_edge;
            while (e != NULL) {
                int v = e->v;
                int w = e->w;
                int new_distance = distances[u] + w;
                if (distances[v] > new_distance) {
                    changed = true;
                    distances[v] = new_distance;
                }
                e = e->next;
            }
        }

        if (!changed)
            break;
    }

    return distances;
}

Result *get_result(Vertex graph[], size_t graph_size,
                   int branches[], size_t num_branches,
                   Edge edges[],   size_t num_edges) {
    int *distances = malloc(sizeof(int)*graph_size);
    int *sum = calloc(graph_size, sizeof(int));
    bool *reachable = malloc(sizeof(bool)*graph_size);
    Result *result = malloc(sizeof(Result));
    int *h = get_reweight_values(graph, graph_size);
    int i;

    result->distances = distances;
    result->meeting_place = -1;
    result->total_loss = INF;

    for (i = 0; i < graph_size; i++) {
        Edge *e = graph[i].first_edge;
        int u = i;
        reachable[i] = true;
        while (e != NULL) {
            int v = e->v;
            int w = e->w;
            e->w = w + h[u] - h[v];
            e = e->next;
        }
    }
    
    for (i = 0; i < num_branches; i++) {
        int j;
        dijkstra(graph, graph_size, branches[i], distances);
        for (j = 0; j < graph_size; j++) {
            if (distances[j] == INF) {
                reachable[j] = false;
                continue;
            }
            sum[j] += distances[j] + h[j] - h[branches[i]];
        }
    }

    for (i = 0; i < graph_size; i++) {
        if (reachable[i] && sum[i] < result->total_loss) {
            result->meeting_place = i;
            result->total_loss = sum[i];
        }
    }

    if (result->meeting_place != -1) {
        transpose_graph(graph, graph_size, edges, num_edges);
        dijkstra(graph, graph_size, result->meeting_place, distances);
        for (i = 0; i < num_branches; i++) {
            distances[branches[i]] += h[result->meeting_place] - h[branches[i]];
        }
    }

    free(sum);
    free(reachable);
    free(h);
    return result;
}

void read_graph(Vertex graph[], size_t graph_size,
                Edge   edges[], size_t num_edges) {
    bool max_defined = false;
    int max_weight = 0;
    int i;

    for (i = 0; i < num_edges; i++) {
        int u, v, w;
        scanf("%d", &u);
        Edge *new_edge = edges + i;
        scanf("%d %d", &v, &w);
        new_edge->u = --u;
        new_edge->v = --v;
        new_edge->w = w;
        new_edge->next = graph[u].first_edge;
        graph[u].first_edge = new_edge;
        if (max_defined)
            max_weight = MAX(max_weight, w);
        else {
            max_defined = true;
            max_weight = w;
        }
    }

    INF = abs(graph_size*max_weight) + 1;
}

int main(int argc, const char *argv[])
{
    size_t graph_size = 0, num_branches = 0, num_edges = 0;
    int i;
    int *branches = NULL;
    Vertex *graph = NULL;
    Edge *edges = NULL;
    Result *result = NULL;

    scanf("%lu %lu %lu", &graph_size, &num_branches, &num_edges);
    branches = malloc(sizeof(*branches)*num_branches);
    graph = calloc(graph_size, sizeof(*graph));
    edges = malloc(sizeof(Edge)*num_edges);

    for (i = 0; i < num_branches; i++) {
        scanf("%d", branches+i);
        branches[i]--;
    }

    read_graph(graph, graph_size, edges, num_edges);

    result = get_result(graph, graph_size,
                        branches, num_branches,
                        edges, num_edges);

    if (result->meeting_place == -1)
        puts("N");
    else {
        printf("%d %d\n", result->meeting_place+1, result->total_loss);
        for (i = 0; i < num_branches; i++) {
            printf("%d ", result->distances[branches[i]]);
        }
        putchar('\n');
    }

    free(result->distances);
    free(result);
    free(branches);
    free(graph);
    free(edges);

    return 0;
}

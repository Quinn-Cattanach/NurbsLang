
#include "mesh.h"

static inline int in_bounds_int(int x, int y, int sx, int sy)
{
    return (x >= 0 && y >= 0 && x < sx && y < sy);
}

static inline void update(
    mesh_t *dest,
    size_t u_index,
    size_t v_index,
    size_t lod_u,
    size_t lod_v,
    vec3_t evaluation)
{
    int numTris_x = (int)(2 * (lod_u - 1));
    int numTris_y = (int)(lod_v - 1);

    int tri_x = (int)u_index * 2;
    int tri_y = (int)v_index;

    vec3_t *result = dest->vertices;

    if (in_bounds_int(tri_x, tri_y, numTris_x, numTris_y))
    {
        uint32_t idx = 3u * (uint32_t)(tri_y * numTris_x + tri_x);
        result[idx + 0] = evaluation;
    }

    tri_x -= 1;
    if (in_bounds_int(tri_x, tri_y, numTris_x, numTris_y))
    {
        uint32_t idx = 3u * (uint32_t)(tri_y * numTris_x + tri_x);
        result[idx + 0] = evaluation;
    }

    tri_x -= 1;
    if (in_bounds_int(tri_x, tri_y, numTris_x, numTris_y))
    {
        uint32_t idx = 3u * (uint32_t)(tri_y * numTris_x + tri_x);
        result[idx + 2] = evaluation;
    }

    tri_y -= 1;
    tri_x += 3;
    if (in_bounds_int(tri_x, tri_y, numTris_x, numTris_y))
    {
        uint32_t idx = 3u * (uint32_t)(tri_y * numTris_x + tri_x);
        result[idx + 1] = evaluation;
    }

    tri_x -= 1;
    if (in_bounds_int(tri_x, tri_y, numTris_x, numTris_y))
    {
        uint32_t idx = 3u * (uint32_t)(tri_y * numTris_x + tri_x);
        result[idx + 1] = evaluation;
    }

    tri_x -= 1;
    if (in_bounds_int(tri_x, tri_y, numTris_x, numTris_y))
    {
        uint32_t idx = 3u * (uint32_t)(tri_y * numTris_x + tri_x);
        result[idx + 2] = evaluation;
    }
}

int tesselate(nurbs_surface_t *surface, size_t lod_u, size_t lod_v, mesh_t **dest)
{
    if (!dest || !*dest || !surface)
        return -1;

    size_t n_vertex = 3 * 2 * (lod_u - 1) * (lod_v - 1);

    if (!(*dest = (mesh_t *)malloc(sizeof(mesh_t))))
    {
        return -1;
    }

    if (!((*dest)->vertices = (vec3_t *)malloc(sizeof(vec3_t) * n_vertex)))
    {
        free(*dest);
        return -1;
    }

    if (!((*dest)->norms = (vec3_t *)malloc(sizeof(vec3_t) * n_vertex)))
    {
        free(*dest);
        free((*dest)->norms);
        return -1;
    }

    for (size_t i = 0; i < lod_u; i += 1)
    {
        for (size_t j = 0; j < lod_v; j += 1)
        {
            vec3_t evaluation;

            evaluate(surface, (double)i / ((double)lod_u - 1), (double)j / ((double)lod_v - 1), &evaluation);

            update(*dest, i, j, lod_u, lod_v, evaluation);
        }
    }
}

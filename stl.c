#include "stl.h"
#include <machine/endian.h>

#if _BYTE_ORDER == _BIG_ENDIAN
    inline float lefloatswap(float value) {
        union { float fval; uint32_t intval; } trans;
        trans.fval = value;
        trans.intval = __bswap32(trans.intval);
        return trans.fval;
    }
#endif

extern int verbose;

void readStlAscii(FILE *f, STL_data *stl) {
    rewind(f);
    uint32_t poly_idx = 0;

    stl->_tris_malloc_size = sizeof(STL_triangle) * 64;
    stl->tris = malloc(stl->_tris_malloc_size);

    printf("\n");
    char poly_buf[255];
    char poly_section[24];
    while ( !feof(f) )
    {
        if (poly_idx + 1 > stl->_tris_malloc_size / sizeof(STL_triangle)) {
            stl->tris = realloc(stl->tris, stl->_tris_malloc_size * 2);
            stl->_tris_malloc_size *= 2;
        }
        if (stl->_tris_malloc_size > 1073741824) {
            printf("STL data exceeds 1GB. Aborting...\n");
            abort();
        }

        fgets(poly_buf, 255, f);
        sscanf(poly_buf, "%s", poly_section);
        if (strcasecmp(poly_section, "facet") == 0)  /* Is this a normal ?? */
        {
            sscanf(poly_buf, "%*s %*s %f %f %f",
                   &(stl->tris[poly_idx]).normal[0],
                   &(stl->tris[poly_idx]).normal[1],
                   &(stl->tris[poly_idx]).normal[2]
            );
        }

        if (strcasecmp(poly_section, "vertex") == 0)  /* Is it a vertex ?  */
        {
            sscanf(poly_buf, "%*s %f %f %f",
                   &(stl->tris[poly_idx]).vertex_a[0],
                   &(stl->tris[poly_idx]).vertex_a[1],
                   &(stl->tris[poly_idx]).vertex_a[2]
            );

            fgets(poly_buf, 255, f);  /* Next two lines vertices also!  */
            sscanf(poly_buf, "%*s %f %f %f",
                   &(stl->tris[poly_idx]).vertex_b[0],
                   &(stl->tris[poly_idx]).vertex_b[1],
                   &(stl->tris[poly_idx]).vertex_b[2]
            );

            fgets(poly_buf, 255, f);
            sscanf(poly_buf, "%*s %f %f %f",
                   &(stl->tris[poly_idx]).vertex_c[0],
                   &(stl->tris[poly_idx]).vertex_c[1],
                   &(stl->tris[poly_idx]).vertex_c[2]
            );

            printf("\033[25m\033[999D\033[1A%u polygons read into %u bytes of memory.\033[K\n\033[0m", poly_idx, stl->_tris_malloc_size);
            poly_idx++;
        }
    }

    stl->tris_size = poly_idx;
}

void readStlBinary(FILE *f, STL_data *stl) {
    rewind(f);
    uint32_t poly_idx = 0;

    stl->_tris_malloc_size = sizeof(STL_triangle) * 64;
    stl->tris = malloc(stl->_tris_malloc_size);

    fread(&stl->header, 80, 1, f);
    fread(&stl->tris_size, 4, 1, f);
    #if _BYTE_ORDER == _BIG_ENDIAN
    stl->tris_size = __bswap32(stl->tris_size);
    #endif

    printf("\n");
    while ( !feof(f) && poly_idx < stl->tris_size ) {
        if (poly_idx + 1 > stl->_tris_malloc_size / sizeof(STL_triangle)) {
            stl->tris = realloc(stl->tris, stl->_tris_malloc_size * 2);
            stl->_tris_malloc_size *= 2;
        }
        if (stl->_tris_malloc_size > 1073741824) {
            printf("STL data exceeds 1GB. Aborting...\n");
            abort();
        }

        int r = fread(&stl->tris[poly_idx], 50, 1, f);
        if (r != 1) break;

        #if _BYTE_ORDER == _BIG_ENDIAN
        for(int pt_idx = 0; pt_idx < 3; pt_idx++) {
            stl->tris[poly_idx].normal[pt_idx]   = lefloatswap(stl->tris[poly_idx].normal[pt_idx]);
            stl->tris[poly_idx].vertex_a[pt_idx] = lefloatswap(stl->tris[poly_idx].vertex_a[pt_idx]);
            stl->tris[poly_idx].vertex_b[pt_idx] = lefloatswap(stl->tris[poly_idx].vertex_b[pt_idx]);
            stl->tris[poly_idx].vertex_c[pt_idx] = lefloatswap(stl->tris[poly_idx].vertex_c[pt_idx]);
        }
        #endif

        if(verbose)
            printf("\033[25m\033[999D\033[1A%u polygons read into %u bytes of memory.\033[K\n\033[0m", poly_idx, stl->_tris_malloc_size);
        poly_idx++;
    }

    printf("stl->tris_size=%u, poly_idx=%u\n",stl->tris_size, poly_idx);
    fread(&stl->tris[poly_idx].attr, 2, 1, f);

}

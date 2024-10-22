/* tree.c */

#include "tree.h"

Tree root = { .n = {
    .tag = (TagRoot | TagNode),
    .north = (Node *)&root,
    .west = 0,
    .east = 0,
    .path = "/"
}};

void print_tree(int fd, Tree *origin) {
   int8 indentation;
   int8 buf[256];
   int16 size;
   Node *n;
   Leaf *l, *last;

   indentation = 0;
   for(n=(Node *)origin; n; n=n->west) {
      Print(indent(indentation++)); 
      Print(n->path); 
      Print("\n");
      if(n->east) {
          last = find_last(n);
          if(last) {
              for(l=last; (Node *)l!=n; l=(Leaf *)l->west) {
                  Print(indent(indentation));
                  Print(n->path);
                  Print("/");
                  Print(l->key);
                  Print(" -> '");
                  write(fd, (char *)l->value, (int)l->size);
                  Print("'\n");
              }
          }
      }
   }
   return;
}

int8 *indent(int8 tabs) {
    static int8 buf[256];
    int8 *p;

    if(tabs < 1) {
        return (int8 *) "";
    }

    assert(tabs < 120); // 256 / 2 = 128 (need \0 at the end) 128 - 1
    zero(buf, 256);

    int16 i;
    for(i=0, p=buf; i<tabs; ++i, p+=2) {
       strncpy((char*)p, "  ", 2);
    }
    return buf;
}

void zero(int8 *str, int16 size) {
    int8 *p;
    int16 n;

    for(n=0, p=str; n<size; ++p, ++n) {
        *p = 0;
    }
    return;
}

Node *create_node(Node *parent, int8 *path) {
    Node *n;
    int16 size;

    assert(parent);
    size = sizeof(struct s_node);
    n = (Node *)malloc((int)size);
    zero((int8 *)n, size);

    parent->west = n;
    n->tag = TagNode;
    n->north = parent;
    strncpy((char*)n->path, (char *)path, 255);

    return n;
}

Node *find_node(int8* path) {
   Node *p, *ret;

   for(ret=(Node *)0, p=(Node *)&root; p; p=p->west) {
        if(!strcmp((char *)p->path, (char *)path)) {
            ret = p;
            break;
        }
   }
   return ret;
}

int8 *lookup_linear(int8 *path, int8 *key) {
    Leaf *p;

    p = find_leaf_linear(path, key);

    return (p) ? p->value : (int8 *)0;
}

Leaf *find_leaf_linear(int8 *path, int8 *key) {
    Node *n;
    Leaf  *l, *ret;

    n = find_node(path);
    if(!n) {
        return (Leaf *)0;
    }
    
    // iterate through the leaves
    for(ret=(Leaf *)0, l=n->east; l; l=l->east) {
        if(!strcmp((char *)l->key, (char *)key)) {
            ret = l;
            break;
        }
    }
    return ret;
}

Leaf *find_last_linear(Node *parent) {
    Leaf *l;

    assert(parent);

    if(!parent->east) {
        return (Leaf *)0;
    }
    for(l=parent->east; l->east; l=l->east);
    assert(l);

    return l;
}

Leaf *create_leaf(Node *parent, int8 *key, int8 *value, int16 count) {
    Leaf *l, *new;
    int16 size;

    assert(parent);
    l = find_last(parent);

    size = sizeof(struct s_leaf);
    new = (Leaf *)malloc(size);
    assert(new);

    
    if(!l) { // directly connected, no hanging leaves
        parent->east = new;
    }
    else { // l is a leaf
        l->east = new; 
    }

    zero((int8 *)new, size);
    new->tag = TagLeaf;
    new->west = (!l) ? (Tree *)parent : (Tree *)l;

    strncpy((char *)new->key, (char *)key, 127);
    new->value = (int8 *)malloc(count);
    zero(new->value, count);
    assert(new->value);
    strncpy((char *)new->value, (char *)value, count);
    new->size = count;

    return new;
}

Tree *generate_test_tree() {
    Node *n, *p;
    int8 c;
    int8 path[256];
    int32 size;

    zero(path, 256);
    size = 0;

    for(n=(Node *)&root, c='a'; c <= 'z'; ++c) {
        size = (int32)strlen((char *)path);
        *(path + size++) = '/';
        *(path + size) = c;

        p = n;
        n = create_node(p, path);
    }

    return (Tree *)&root;
}

int8 *generate_test_path(int8 path) {
    static int8 buf[256];
    int8 c;
    int32 counter;

    zero(buf, 256);
    for(c='a'; c<=path; ++c) {
        counter = (int32)strlen((char *)buf);
        *(buf + counter++) = '/';
        *(buf + counter) = c;
    }
    return buf;
}

int8 *test_duplicate(int8 *str) {
    static int8 buf[256];
    int16 n;

    zero(buf, 256);
    strncpy((char *)buf, (char *)str, 255);  
    n = (int16)strlen((char *)buf);  
    
    if((n * 2) > 255) {
        return buf;
    }

    for (int16 i = 0; i < n; i++) {
        buf[n + i] = buf[i];
    }

    return buf;
}

int32 generate_test_leaves() {
    FILE* fd;
    int32 c_left, generated_leaves;
    int8 buf[256];
    int8 *path, *value;
    Node *n;

    fd = fopen(ExampleFile, "r");
    assert(fd);

    zero(buf, 256);
    generated_leaves = 0;
    while(fgets((char *)buf, 255, fd)) {
        c_left = (int32)strlen((char *)buf);
        *(buf + c_left - 1) = 0;
        path = generate_test_path(*buf);
        n = find_node(path);
        if(!n) {
            zero(buf, 256);
            continue;
        }
        value = test_duplicate(buf);
        create_leaf(n, buf, value, (int16)strlen((char *)value));
        ++generated_leaves;
        zero(buf, 256);
    }
    fclose(fd);

    return generated_leaves;
}

int main() {
    Tree *test;
    int32 leaves_count;

    test = generate_test_tree();

    printf("Populating tree...\n");
    fflush(stdout);

    leaves_count = generate_test_leaves();
    printf("Done! Generated: %d leaves\n", leaves_count);

    print_tree(1, test);    

    return 0;
}
#pragma GCC diagnostic pop


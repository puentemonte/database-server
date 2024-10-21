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
              for(l=last; (Node *)l->west!=n; l=(Leaf *)l->west) {
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

    zero((int8 *)new, size);
    new->tag = TagLeaf;

    if(!l) { // directly connected, no hanging leaves
        parent->east = new;
        new->west = (Tree *)parent;
    }
    else { // l is a leaf
        l->east = new; 
        new->west = (Tree *)l;
    }

    strncpy((char *)new->key, (char *)key, 127);
    new->value = (int8 *)malloc(count);
    zero(new->value, count);
    assert(new->value);
    strncpy((char *)new->value, (char *)value, count);
    new->size = count;

    return new;
}

int main() {
    Node *n, *n2;
    Leaf *l1, *l2;
    int8 *key, *value;
    int16 size;

    n = create_node((Node *)&root, (int8 *)"/Users"); 
    assert(n);
    n2 = create_node(n, (int8 *)"Users/login");
    assert(n2);
    
    key = (int8 *)"erik";
    value = (int8 *)"abc77301aa";
    size = (int16)strlen((char *)value);
    l1 = create_leaf(n2, key, value, size);
    assert(l1);

    key = (int8 *)"elena";
    value = (int8 *)"aa034945c";
    size = (int16)strlen((char *)value);
    l2 = create_leaf(n2, key, value, size);
    assert(l2);

    print_tree(1, &root);

    free(l2);
    free(l1);
    free(n2);
    free(n);
 
    return 0;
}
#pragma GCC diagnostic pop


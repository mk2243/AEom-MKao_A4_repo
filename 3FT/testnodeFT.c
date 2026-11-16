/* testnodeFT.c -- test client for nodeFT.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodeFT.h"

static int checks = 0;

#define CHECK(expr) do {                                                \
   checks++;                                                            \
   if (!(expr)) {                                                       \
      fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #expr);      \
      exit(EXIT_FAILURE);                                               \
   }                                                                    \
} while (0)

/* Create a Path_T, create a node from it, and free the temporary path. */
static Node_T makeNode(const char *name, Node_T parent, boolean isFile,
                       void *contents, size_t length)
{
   Path_T path = NULL;
   Node_T node = NULL;
   CHECK(Path_new(name, &path) == SUCCESS);
   CHECK(Node_new(path, parent, isFile, contents, length, &node) == SUCCESS);
   CHECK(node != NULL);
   Path_free(path);
   return node;
}

/* Check an unsuccessful creation and its output parameter. */
static void expectNewError(const char *name, Node_T parent,
                           boolean isFile, int expected)
{
   Path_T path = NULL;
   Node_T result = parent;  /* May be non-NULL before the call. */
   CHECK(Path_new(name, &path) == SUCCESS);
   CHECK(Node_new(path, parent, isFile, NULL, 0, &result) == expected);
   CHECK(result == NULL);
   Path_free(path);
}

/* Check child lookup and its returned insertion/found index. */
static void expectHasChild(Node_T parent, const char *name,
                           boolean expected, size_t expectedIndex)
{
   Path_T path = NULL;
   size_t index = (size_t)-1;
   CHECK(Path_new(name, &path) == SUCCESS);
   CHECK(Node_hasChild(parent, path, &index) == expected);
   CHECK(index == expectedIndex);
   Path_free(path);
}

int main(void)
{
   Node_T root, alpha, middle, zeta, readme, subdir, leaf, result;
   char initialContents[] = "hello";
   unsigned char newContents[] = { 'A', '\0', 'B' };
   char *name;

   /* 1. Root creation and invalid roots. */
   expectNewError("root/file", NULL, FALSE, NO_SUCH_PATH);
   expectNewError("fileRoot", NULL, TRUE, CONFLICTING_PATH);
   root = makeNode("root", NULL, FALSE, NULL, 0);
   CHECK(Node_isFile(root) == FALSE);
   CHECK(Node_getParent(root) == NULL);
   CHECK(Node_getNumChildren(root) == 0);
   CHECK(strcmp(Path_getPathname(Node_getPath(root)), "root") == 0);

   /* 2. Children inserted out of order must be stored in path order. */
   zeta = makeNode("root/zeta", root, FALSE, NULL, 0);
   middle = makeNode("root/middle", root, TRUE,
                     initialContents, strlen(initialContents));
   alpha = makeNode("root/alpha", root, FALSE, NULL, 0);
   CHECK(Node_getNumChildren(root) == 3);
   CHECK(Node_getParent(alpha) == root);
   CHECK(Node_getParent(middle) == root);
   CHECK(Node_isFile(middle) == TRUE);
   CHECK(Node_isFile(alpha) == FALSE);

   result = NULL;
   CHECK(Node_getChild(root, 0, &result) == SUCCESS);
   CHECK(result == alpha);
   CHECK(Node_getChild(root, 1, &result) == SUCCESS);
   CHECK(result == middle);
   CHECK(Node_getChild(root, 2, &result) == SUCCESS);
   CHECK(result == zeta);

   expectHasChild(root, "root/alpha", TRUE, 0);
   expectHasChild(root, "root/middle", TRUE, 1);
   expectHasChild(root, "root/aardvark", FALSE, 0);
   expectHasChild(root, "root/beta", FALSE, 1);
   expectHasChild(root, "root/zzzz", FALSE, 3);

   result = root;
   CHECK(Node_getChild(root, 3, &result) == NO_SUCH_PATH);
   CHECK(result == NULL);
   CHECK(Node_compare(alpha, middle) < 0);
   CHECK(Node_compare(middle, zeta) < 0);
   CHECK(Node_compare(alpha, alpha) == 0);

   /* 3. Nested directories and files. */
   readme = makeNode("root/alpha/readme", alpha, TRUE, NULL, 0);
   subdir = makeNode("root/alpha/subdir", alpha, FALSE, NULL, 0);
   leaf = makeNode("root/alpha/subdir/leaf", subdir, TRUE, NULL, 0);
   CHECK(Node_getNumChildren(alpha) == 2);
   CHECK(Node_getNumChildren(subdir) == 1);
   CHECK(Node_getParent(readme) == alpha);
   CHECK(Node_getParent(leaf) == subdir);

   /* 4. Invalid child creation. */
   expectNewError("root/alpha", root, FALSE, ALREADY_IN_TREE);
   expectNewError("root/other", alpha, FALSE, CONFLICTING_PATH);
   expectNewError("root/alpha/missing/deep", alpha,
                  FALSE, NO_SUCH_PATH);
   expectNewError("root/middle/new", middle, FALSE, NOT_A_DIRECTORY);

   /* 5. File-only and directory-only operations. */
   CHECK(Node_getNumChildren(middle) == 0);
   result = root;
   CHECK(Node_getChild(middle, 0, &result) == NOT_A_DIRECTORY);
   CHECK(result == NULL);
   CHECK(Node_getFileContents(middle) == initialContents);
   CHECK(Node_getFileLength(middle) == strlen(initialContents));
   CHECK(Node_replaceFileContents(middle, newContents,
                                  sizeof(newContents)) == initialContents);
   CHECK(Node_getFileContents(middle) == newContents);
   CHECK(Node_getFileLength(middle) == sizeof(newContents));
   CHECK(memcmp(Node_getFileContents(middle), newContents,
                sizeof(newContents)) == 0);
   CHECK(Node_replaceFileContents(middle, NULL, 0) == newContents);
   CHECK(Node_getFileContents(middle) == NULL);
   CHECK(Node_getFileLength(middle) == 0);

   /* 6. String conversion: caller must free the returned string. */
   name = Node_toString(readme);
   CHECK(name != NULL);
   CHECK(strcmp(name, "root/alpha/readme") == 0);
   free(name);

   /* 7. Recursive deletion updates the parent's child list. */
   CHECK(Node_free(alpha) == 4); /* alpha, readme, subdir, leaf */
   CHECK(Node_getNumChildren(root) == 2);
   expectHasChild(root, "root/alpha", FALSE, 0);
   CHECK(Node_getChild(root, 0, &result) == SUCCESS);
   CHECK(result == middle);
   CHECK(Node_free(root) == 3); /* root, middle, zeta */

   printf("All %d checks passed.\n", checks);
   return EXIT_SUCCESS;
}

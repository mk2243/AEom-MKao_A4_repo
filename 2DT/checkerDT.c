/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"
#include "dt.h"



/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T oNNode) {
   Node_T oNParent;
   Path_T oPNPath;
   Path_T oPPPath;
   size_t ulIndex;

   /* Sample check: a NULL pointer is not a valid node */
   if(oNNode == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   oPNPath = Node_getPath(oNNode);
   if(oPNPath == NULL){
      fprintf(stderr, "A node has a NULL path\n");
      return FALSE;
   }

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   oNParent = Node_getParent(oNNode);
   if(oNParent != NULL) {
      oPPPath = Node_getPath(oNParent);

      if(oPPPath == NULL || Path_getSharedPrefixDepth(oPNPath, oPPPath) != 
      Path_getDepth(oPNPath)-1){
         fprintf(stderr, "Invalid parent-child path relationship\n");
         return FALSE;
      }
   } else if(Path_getDepth(oPNPath) != 1){
      fprintf(stderr, "Root node does not have depth 1\n");
      return FALSE;
   }

      for (ulIndex = 1; ulIndex < Node_getNumChildren(oNNode); ulIndex++){
      Node_T oNChild = NULL;
      if (Node_getChild(oNNode, ulIndex, &oNChild) != SUCCESS){
         fprintf(stderr, "Invalid child index\n");
         return FALSE;
      }
      
      if(oNChild == NULL){
         fprintf(stderr, "Child is NULL\n");
         return FALSE;
      }

      if(Node_getParent(oNChild) != oNNode){
         fprintf(stderr, "Child has incorrect parent pointer\n");
         return FALSE;
      }
   }

   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static boolean CheckerDT_treeCheck(Node_T oNNode) {
   size_t ulIndex;
   Node_T oNPrevChild;

   if(oNNode!= NULL) {
      Node_T *children;
      /*
      char *ps;
      char c;
      size_t count;
      */

      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerDT_Node_isValid(oNNode))
         return FALSE;

      /* ps = DT_toString();
      c = *ps;
      count = 0;
      while ((c = *ps)!='\0') {
         if (c == '\\') {
            count++;
            ps++;
            }
      } 
      if (count != DT_get */

      children = calloc(Node_getNumChildren(oNNode), sizeof(*children));

      /* Recur on every child of oNNode */
      for(ulIndex = 0; ulIndex < Node_getNumChildren(oNNode); ulIndex++)
      {
         Node_T oNChild = NULL;

         int iStatus = Node_getChild(oNNode, ulIndex, &oNChild);
         size_t i;

         if(iStatus != SUCCESS) {
            fprintf(stderr, "getNumChildren claims more children than " 
               "getChild returns\n");
            return FALSE;
         }

         /* 
         if (strlen(DT_toString()) >= strlen(Node_toString(oNNode))) {
            fprintf(stderr, "String representation of DT is shorter \
               than string representation of one of its nodes\n");
            return FALSE;
         }
         */

         /* Check if children of oNNode are stored in lexicographic
         order by comparing their string representations */
         if (ulIndex > 0) {
            char *pcCurrString = Node_toString(oNChild);
            char *pcPrevString = Node_toString(oNPrevChild);
            /* (void) Node_getChild(oNNode, ulIndex-1, &oNPrevChild); */
            if (strcmp(pcPrevString, pcCurrString) > 0) {
               fprintf(stderr, "Children of a node in DT is are "
                  "not stored in lexicographic order\n");
               return FALSE;
            }
            free(pcPrevString);
            free(pcCurrString);
         }

         /* Check if oNChild has the same path as any of the previously
         visited children of oNNode */
         for(i = 0; i < ulIndex; i++) {
            if (Node_compare(children[i], oNChild) == 0) {
               fprintf(stderr, "Two nodes of the same path exist in DT"
                   "\n");
               free(children);
               return FALSE;
            }
         }
         children[ulIndex] = oNChild;

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(!CheckerDT_treeCheck(oNChild))
            return FALSE;

         oNPrevChild = oNChild;
      }
   }
   return TRUE;
}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount) {

   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */
   if(!bIsInitialized)
      if(ulCount != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }

   /* Now checks invariants recursively at each node from the root. */
   return CheckerDT_treeCheck(oNRoot);
}

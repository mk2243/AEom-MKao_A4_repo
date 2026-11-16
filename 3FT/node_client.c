/*--------------------------------------------------------------------*/
/* node_client.c                                                      */
/* Author: Mandy Kao                                                  */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "nodeFT.h"
#include "path.h"
#include "a4def.h"

int main(void)
{
   Node_T oNRoot;
   Node_T oNResult;
   Path_T oPPath;
   char *pcString;
   size_t ulCount;
   size_t ulIndex;
   int iStatus;

   char acContents[] = "Hello, world!";
   char acNewContents[] = "Updated contents";
   void *pvOldContents;

   printf("Starting Node module tests...\n\n");

   /*--------------------------------------------------------------*/
   /* TEST 1: Create a root directory                               */
   /*--------------------------------------------------------------*/

   printf("TEST 1: Creating root directory\n");

   assert(Path_new("/root", &oPPath) == SUCCESS);

   iStatus = Node_new(oPPath, NULL, FALSE, NULL, 0, &oNRoot);

   assert(iStatus == SUCCESS);
   assert(oNRoot != NULL);

   Path_free(oPPath);

   assert(Node_isFile(oNRoot) == FALSE);
   assert(Node_getParent(oNRoot) == NULL);
   assert(Node_getNumChildren(oNRoot) == 0);

   printf("PASS: Root directory created.\n\n");


   /*--------------------------------------------------------------*/
   /* TEST 2: Get root path                                         */
   /*--------------------------------------------------------------*/

   printf("TEST 2: Getting root path\n");

   pcString = Node_toString(oNRoot);

   assert(pcString != NULL);
   assert(strcmp(pcString, "/root") == 0);

   printf("Root path: %s\n", pcString);

   free(pcString);

   printf("PASS: Root path retrieved.\n\n");


   /*--------------------------------------------------------------*/
   /* TEST 3: Check root children                                   */
   /*--------------------------------------------------------------*/

   printf("TEST 3: Checking root children\n");

   assert(Node_getNumChildren(oNRoot) == 0);

   assert(Path_new("/root/missing", &oPPath) == SUCCESS);

   assert(Node_hasChild(oNRoot, oPPath, &ulIndex) == FALSE);

   Path_free(oPPath);

   printf("PASS: Empty root checked correctly.\n\n");


   /*--------------------------------------------------------------*/
   /* TEST 4: Invalid child index                                   */
   /*--------------------------------------------------------------*/

   printf("TEST 4: Testing invalid child index\n");

   iStatus = Node_getChild(oNRoot, 0, &oNResult);

   assert(iStatus == NO_SUCH_PATH);
   assert(oNResult == NULL);

   printf("PASS: Invalid child index handled correctly.\n\n");


   /*--------------------------------------------------------------*/
   /* TEST 5: Invalid root creation                                  */
   /*--------------------------------------------------------------*/

   printf("TEST 5: Testing invalid root creation\n");

   /* A root cannot be a file. */
   assert(Path_new("/file", &oPPath) == SUCCESS);

   iStatus = Node_new(oPPath, NULL, TRUE,
                      acContents, strlen(acContents), &oNResult);

   assert(iStatus == CONFLICTING_PATH);
   assert(oNResult == NULL);

   Path_free(oPPath);

   /* A root must have depth 1. */
   assert(Path_new("/root/subfolder", &oPPath) == SUCCESS);

   iStatus = Node_new(oPPath, NULL, FALSE,
                      NULL, 0, &oNResult);

   assert(iStatus == NO_SUCH_PATH);
   assert(oNResult == NULL);

   Path_free(oPPath);

   printf("PASS: Invalid root creation handled correctly.\n\n");


   /*--------------------------------------------------------------*/
   /* TEST 6: Free root                                             */
   /*--------------------------------------------------------------*/

   printf("TEST 6: Freeing root node\n");

   ulCount = Node_free(oNRoot);

   assert(ulCount == 1);

   printf("Number of nodes freed: %lu\n",
          (unsigned long)ulCount);

   printf("PASS: Root freed successfully.\n\n");


   printf("All available Node tests passed!\n");

   return 0;
}
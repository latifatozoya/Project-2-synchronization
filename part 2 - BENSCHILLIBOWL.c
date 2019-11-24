#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* mcg);
bool IsFull(BENSCHILLIBOWL* mcg);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    return BENSCHILLIBOWLMenu[rand() % BENSCHILLIBOWLMenuLength];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL *restaurant = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
 
    //initialize variables
    restaurant->orders = NULL;
    restaurant->current_size = 0;
    restaurant->max_size = max_size;
    restaurant->next_order_number = 1;
    restaurant->orders_handled = 0;
    restaurant->expected_num_orders = expected_num_orders;

    pthread_mutex_init(&(restaurant->mutex), NULL); //initialize mutex
    pthread_cond_init(&(restaurant->can_add_orders), NULL); //initialize conditions
    pthread_cond_init(&(restaurant->can_get_orders), NULL);

    printf("Restaurant is open!\n");
    return restaurant;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* mcg) {
   printf("Orders handled: %d\n", mcg->orders_handled);
   printf("Orders expected: %d\n", mcg->expected_num_orders);
   //check condition
   if(mcg->orders_handled != mcg->orders_handled){
      fprintf(stderr, "Not all orders handled");
      exit(0);
   }
   pthread_mutex_destroy(&(mcg->mutex));
   free(mcg); //delete restaurant
   printf("Restaurant is closed!\n");
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* mcg, Order* order) {
   pthread_mutex_lock(&(mcg->mutex));
   while (IsFull(mcg)){
   pthread_cond_wait(&(mcg->can_add_orders), &(mcg->mutex));
   }
   
   order->order_number = mcg->next_order_number;
   AddOrderToBack(&(mcg->orders), order);
   mcg->next_order_number++;
   mcg->current_size++;
   pthread_cond_broadcast(&(mcg->can_get_orders));
   pthread_mutex_unlock(&(mcg->mutex));
   return order->order_number;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* mcg) {
   pthread_mutex_lock(&(mcg->mutex));
   while (mcg->current_size==0) {
      if (mcg->orders_handled >= mcg->expected_num_orders) {
            pthread_mutex_unlock(&(mcg->mutex));
            return NULL;
       }
       pthread_cond_wait(&(mcg->can_get_orders), &mcg->mutex);
   }
  
   Order *order = mcg->orders;
   mcg->orders = mcg->orders->next;
   mcg->current_size--;
   mcg->orders_handled++;

   pthread_cond_broadcast(&(mcg->can_add_orders));
   pthread_mutex_unlock(&(mcg->mutex));
   return order;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* mcg) {
 return mcg->current_size == 0;
}

bool IsFull(BENSCHILLIBOWL* mcg) {
   return mcg->current_size == mcg->max_size;
}

/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
   if(*orders == NULL) {
     *orders = order;
   } else{
     Order *temp = *orders;
     while (temp->next){
     temp = temp->next;
     }
     temp->next = order;
   }
}

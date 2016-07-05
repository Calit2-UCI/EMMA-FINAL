/*
 * NaradaNotificationCenter.cpp
 *
 *  Created on: 09/01/2012
 *  Author: narada
 */

#include "NaradaNotificationCenter.hpp"
#include <string>

//namespace VisageSDK
//{

    
    NaradaNotificationCenter* NaradaNotificationCenter::center = NULL;


	/**
	 * @brief Constructor: Loads an AFM model to represent the virtual character. Then initializes
	 * the player with the loaded model.
	 *
	 * @param afmPath   string to the file with the AFM model to load, as a representation of virtual character
	 * @param perspTanFOVY   float with tan(FOVY) of the perspective. It has a parameter tan(45/2) as default value.
	 */
  
    NaradaNotificationCenter::NaradaNotificationCenter(){}
    
    
    
    
    /**
	 * @brief Destructor: destroys the NaradaNotificationCenter object
     *As we want to have a unique instance during all the runtime, the notification center will free it's memory only when exits the program
	 *
	 */
    NaradaNotificationCenter::~NaradaNotificationCenter(){}
    
    
    
    
    /**
	 * @brief Post a notification.
     * All the handlers subscribed to this notification, will be called
     *
	 * @param *notification pointer to std::string with notification
     *
     *
     * @see subscribeNotification, unsubscribeNotification
     *
     * Example usage:
     * @code
     *  void subscriberFunction(){ std::cout << "Hello world!" << std::endl; }
     *
     *  //in my code...
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, &subscriberFunction);
     *
     *  //...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber); //in another thread, this will write by console "Hello world!"
     *
     *  //When I want to cancel the subscription
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, &subscriberFunction);
     *  @endcode
     *
	 */
    void NaradaNotificationCenter::postNotification(std::string notification)
    {
        checkInitCenter();
//        std::cout << "Posting notification: " << notification << std::endl;
        std::vector<NaradaNotificationRelationBase*> pendingNotif = findActiveNotifications(notification);
        executeHandlers(pendingNotif);

    }
    
    

    /**
     * @brief Post a notification, passing some integer data that can be consumed by the subscribers
     * All the handlers subscribed to this notification, will be called
     *
     * @param *notification pointer to std::string with notification
     *
     *
     * @see subscribeNotification, unsubscribeNotification
     *
     * Example usage:
     * @code
     *  void subscriberFunction(){ std::cout << "Hello world!" << std::endl; }
     *
     *  //in my code...
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, &subscriberFunction);
     *
     *  //...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber, 5); //in another thread, this will write by console "Hello world!"
     *
     *  //When I want to cancel the subscription
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, &subscriberFunction);
     *  @endcode
     *
     */
    void NaradaNotificationCenter::postNotification(std::string notification, int data)
    {
        checkInitCenter();
//        std::cout << "Posting notification: " << notification << std::endl;
        std::vector<NaradaNotificationRelationBase*> pendingNotif = findActiveNotifications(notification);
        for(int i=0; i< pendingNotif.size(); i++)
            pendingNotif.at(i)->setNotificationReportedData(data);
        executeHandlers(pendingNotif);
        
    }



    /**
	 * @brief Subscribe notification, C styled.
     * When the notification is posted, the function passed as handler argument will be executed
     *
	 * @param *notification pointer to std::string with notification
	 * @param void (*handler)() pointer to a void function with the handler
     *
     * @see unsubscribeNotification, postNotification
     *
     * Example usage:
     * @code
     *  void subscriberFunction(){ std::cout << "Hello world!" << std::endl; }
     *
     *  //in my code...
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, &subscriberFunction);
     *
     *  //...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber); //in another thread, this will write by console "Hello world!"
     *
     *  //When I want to cancel the subscription
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, &subscriberFunction);
     *  @endcode
     *
	 */
    void NaradaNotificationCenter::subscribeNotification(std::string notification, void (*handler)())
    {
        checkInitCenter();
        NaradaNotificationRelationCStyled *notifRelation = new NaradaNotificationRelationCStyled(notification, *handler);
        center->notificationsTable.push_back(notifRelation);

        std::cout << "subscrived handler for notification: " << notification.c_str() << std::endl;

    }



    /**
     * @brief Subscribe notification with int argument, C styled.
     * When the notification is posted, the function passed as handler argument with int paramenter will be executed
     *
     * @param *notification pointer to std::string with notification
     * @param void (*handler)(int data) pointer to a void function with int argument that will be used as a handler
     *
     * @see unsubscribeNotification, postNotification
     *
     * Example usage:
     * @code
     *   void subscriberFunction(int data){ for(int i=0; i<data; i++) {std::cout << "Hello world!" << std::endl;} }
     *
     *  //in my code...
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, &subscriberFunction);
     *
     *  //...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber, 5); //in another thread, this will write by console "Hello world!" 5 times
     *
     *  //When I want to cancel the subscription
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, &subscriberFunction);
     *  @endcode
     *
     */
    void NaradaNotificationCenter::subscribeNotification(std::string notification, void (*handler)(int data))
    {
        checkInitCenter();
        NaradaNotificationRelationCStyled *notifRelation = new NaradaNotificationRelationCStyled(notification, *handler);
        center->notificationsTable.push_back(notifRelation);
        
        std::cout << "subscrived handler for notification: " << notification.c_str() << std::endl;
        
    }



    /**
	 * @brief Unsubscribe notification.
     * Deletes the relation between the handler and the notification, so, when the notification is posted, the function passed as handler argument
     * won't be executed anymore
     *
	 * @param *notification pointer to std::string with notification
	 * @param void (*handler)() pointer to a void function with the handler
     *
     * @see subscribeNotification, postNotification
     *
     * Example usage:
     * @code
     *  void subscriberFunction(){ std::cout << "Hello world!" << std::endl; }
     *
     *  //in my code...
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, &subscriberFunction);
     *
     *  //...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber); //in another thread, this will write by console "Hello world!"
     *
     *  //When I want to cancel the subscription
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, &subscriberFunction);
     *  @endcode
     *
	 */
    void NaradaNotificationCenter::unsubscribeNotification(std::string notification, void (*handler)())
    {
        checkInitCenter();
        removeNotificationRelation(notification, *handler);
        
    }



    /**
     * @brief Unsubscribe notification.
     * Deletes the relation between the handler and the notification, so, when the notification is posted, the function passed as handler argument
     * won't be executed anymore
     *
     * @param *notification pointer to std::string with notification
     * @param void (*handler)(int data) pointer to a void function with int argument that will be used as a handler
     *
     * @see subscribeNotification, postNotification
     *
     * Example usage:
     * @code
     *   void subscriberFunction(int data){ for(int i=0; i<data; i++) {std::cout << "Hello world!" << std::endl;} }
     *
     *  //in my code...
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, &subscriberFunction);
     *
     *  //...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber, 5); //in another thread, this will write by console "Hello world!" 5 times
     *
     *  //When I want to cancel the subscription
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, &subscriberFunction);
     *  @endcode
     *
     */
    void NaradaNotificationCenter::unsubscribeNotification(std::string notification, void (*handler)(int data))
    {
        checkInitCenter();
        removeNotificationRelation(notification, *handler);
        
    }



    /**
	 * @brief Unsubscribe all notifications.
     * Deletes the relation between any handler and its associated notification
     *
     * @see subscribeNotification, unsubscribeNotification, postNotification
     *
	 */
    void NaradaNotificationCenter::unsubscribeAll()
    {
        checkInitCenter();
        center->notificationsTable.clear();
        
    }
    
    
    
    /**
	 * @brief Get the notifications that have at least one handler assigned
     * Return a vector with the active notifications and their assigned handlers
     *
	 * @param *notification pointer to std::string with notification
	 * @return vector<NaradaNotificationRelationCStyled*> with the active notifications
     *
	 */
    std::vector<NaradaNotificationRelationBase*> NaradaNotificationCenter::findActiveNotifications(std::string notification)
    {
        std::vector<NaradaNotificationRelationBase*> results;
        
        for(int i=0; i<center->notificationsTable.size(); i++)
        {
            if(!notification.compare(center->notificationsTable.at(i)->getNotification()))
            {
                results.push_back(center->notificationsTable.at(i));
            }
        }
        
        return results;
    }
    
    

    
    /**
	 * @brief Execute the handlers from the notification relations passed as argument
     *
	 * @param vector<NaradaNotificationRelationBase*> pendingNotif, vector with with the notification relations whose handler whe we want to execute
     *
	 */
    void NaradaNotificationCenter::executeHandlers(std::vector<NaradaNotificationRelationBase*> pendingNotif)
    {
        if (pendingNotif.size())
        {
            for(int i=0; i< pendingNotif.size(); i++)
            {
                pthread_t handlerThread;
                
                int threadCreated = -1;
                
                threadCreated = pthread_create(&handlerThread, NULL, NaradaNotificationRelationBase::callHandler, (void*)pendingNotif.at(i)/*args*/);

                
                if(threadCreated)
                {
                    printf("Can't create handler pthread!\n");
                    return;
                }
                
                pthread_detach(handlerThread);
                
            }
        }
    }
    
    
    
    
    /**
	 * @brief Remove notification relation
     * Remove the notification relations whose handler match with the handler argument for the specified notification
     *
	 * @param *notification pointer to std::string with notification
	 * @param void (*handler)() pointer to a void function with the handler
     *
	 */
    void NaradaNotificationCenter::removeNotificationRelation(std::string notification, void (*handler)())
    {
        
        for(int i=0; i<center->notificationsTable.size(); i++)
        {
            if(!notification.compare(center->notificationsTable.at(i)->getNotification()))
            {
                NaradaNotificationRelationCStyled *storedSubscriber = dynamic_cast<NaradaNotificationRelationCStyled*>(center->notificationsTable.at(i));
                if(storedSubscriber)
                {
                    if(handler == storedSubscriber->getHandler())
                    {
                        center->notificationsTable.erase(center->notificationsTable.begin()+i);
                        i--;
                    }
                }
            }
        }
        
    }


    /**
     * @brief Remove notification relation
     * Remove the notification relations whose handler match with the handler argument for the specified notification
     *
     * @param *notification pointer to std::string with notification
     * @param void (*handler)(int data) pointer to a void function with int argument, that will be the handler
     *
     */
    void NaradaNotificationCenter::removeNotificationRelation(std::string notification, void (*handler)(int data))
    {
        
        for(int i=0; i<center->notificationsTable.size(); i++)
        {
            if(!notification.compare(center->notificationsTable.at(i)->getNotification()))
            {
                NaradaNotificationRelationCStyled *storedSubscriber = dynamic_cast<NaradaNotificationRelationCStyled*>(center->notificationsTable.at(i));
                if(storedSubscriber)
                {
                    if(handler == storedSubscriber->getHandlerWithData())
                    {
                        center->notificationsTable.erase(center->notificationsTable.begin()+i);
                        i--;
                    }
                }
            }
        }
        
    }




    /**
	 * @brief Check if the NaradaNotificationCenter static member center has been instantiated or not
     * if it does not exist, a new instance is created
     *
     *
	 */
    void NaradaNotificationCenter::checkInitCenter()
    {
        if (center == NULL)
        {
            center = new NaradaNotificationCenter();
        }
    }
    

//}

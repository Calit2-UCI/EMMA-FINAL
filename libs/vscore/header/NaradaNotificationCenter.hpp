/*
 * NaradaNotificationCenter.h
 *
 *  Created on: 09/01/2012
 *      Author: narada
 */

#ifndef NARADA_NOTIFICATION_CENTER_H
#define NARADA_NOTIFICATION_CENTER_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "NaradaException.h"
#include "NaradaNotificationMessages.h"

#ifndef MSVC
#include <pthread.h>
#endif


#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else
	#if defined(MAC) || defined(LINUX)
		#define VISAGE_DECLSPEC __attribute__((__visibility__("default")))

	#else
		#ifdef VISAGE_EXPORTS
			#define VISAGE_DECLSPEC __declspec(dllexport)
		#else
			#define VISAGE_DECLSPEC __declspec(dllimport)
		#endif
	#endif

#endif


//namespace VisageSDK
//{

/**
 * @class NaradaNotificationRelationBase
 * @brief Internal class for NaradaNotificationCenter usage, that allows to relate abstractly notifications with handlers
 * no matter if the subscriber is a C independent function or a C++ class method. See implementations in related classes:
 *
 * @see NaradaNotificationRelationCStyled, NaradaNotificationCPPStyled
 *
 */
class VISAGE_DECLSPEC NaradaNotificationRelationBase
{
    protected:
        std::string notification;
        int notificationReportedData;
    
    public:
    
        NaradaNotificationRelationBase()
        {
            notificationReportedData = 0;
        }
    
        void setNotificationReportedData(int data){notificationReportedData = data;};
    
        int getNotificationReportedData(){return notificationReportedData;};
    
        virtual void useHandler()= 0;
        virtual void useHandlerWithData()= 0;
    
        std::string getNotification(){return notification;};
    
        static void *callHandler(void* args)
        {
            NaradaNotificationRelationBase* relation = (NaradaNotificationRelationBase*) args;
            relation->useHandler();
            relation->useHandlerWithData();
            pthread_exit(NULL);
            
        };
    
};
    
    

/**
 * @class NaradaNotificationRelationCStyled
 * @brief Implementation of NaradaNotificationRelationBase for C style subscribers.
 * Internal class for NaradaNotificationCenter usage, that allows to relate notifications with handlers
 *
 * @see NaradaNotificationRelationBase
 *
 */
class VISAGE_DECLSPEC NaradaNotificationRelationCStyled: public NaradaNotificationRelationBase
{
    typedef void (*naradaNotificationHandlerFunc)();
    typedef void (*naradaNotificationHandlerFuncWithData)(int data);
    
    
    protected:
    

        naradaNotificationHandlerFunc handler;
        naradaNotificationHandlerFuncWithData handlerWithData;
    
    
    public:
        NaradaNotificationRelationCStyled(std::string notif, naradaNotificationHandlerFunc func):NaradaNotificationRelationBase()
        {
            notification = notif;
            handler = func;
            handlerWithData = NULL;
        }

        NaradaNotificationRelationCStyled(std::string notif, naradaNotificationHandlerFuncWithData func):NaradaNotificationRelationBase()
        {
            notification = notif;
            handlerWithData = func;
            handler = NULL;
        }
    
        ~NaradaNotificationRelationCStyled()
        {
            handler = NULL;
            handlerWithData = NULL;
        };
    
    
        void useHandler()
        {
            if(handler)
                handler();
        }

        void useHandlerWithData()
        {
            if(handlerWithData)
                handlerWithData(notificationReportedData);
        }
    
        naradaNotificationHandlerFunc getHandler()
        {
            return handler;
        }
    
        naradaNotificationHandlerFuncWithData getHandlerWithData()
        {
            return handlerWithData;
        }
    
};



/**
 * @class NaradaNotificationRelationCPPStyled
 * @brief Implementation of NaradaNotificationRelationBase for C++ style subscribers.
 * Internal class for NaradaNotificationCenter usage, that allows to relate notifications with handlers
 *
 * @see NaradaNotificationRelationBase
 *
 */
template <typename NRSubscriber>
class VISAGE_DECLSPEC NaradaNotificationRelationCPPStyled : public NaradaNotificationRelationBase
{
    typedef void (NRSubscriber::*naradaNotificationHandlerFunc)();
    typedef void (NRSubscriber::*naradaNotificationHandlerFuncWithData)(int arg);
        
    protected:
        
        naradaNotificationHandlerFunc handler;
        naradaNotificationHandlerFuncWithData handlerWithData;
        NRSubscriber* objectSubscriber;
    
    
    public:
        NaradaNotificationRelationCPPStyled(std::string notif, NRSubscriber *subscriber, naradaNotificationHandlerFunc func):NaradaNotificationRelationBase()
        {
            notification = notif;
            handler = func;
            handlerWithData = NULL;
            objectSubscriber = subscriber;
        }
    
        NaradaNotificationRelationCPPStyled(std::string notif, NRSubscriber *subscriber, naradaNotificationHandlerFuncWithData func):NaradaNotificationRelationBase()
        {
            notification = notif;
            handlerWithData = func;
            handler = NULL;
            objectSubscriber = subscriber;
        }
    
    
        ~NaradaNotificationRelationCPPStyled()
        {
            objectSubscriber = NULL;
            handler = NULL;
            handlerWithData = NULL;
        };
        
        
        void useHandler()
        {
            if(handler)
                ((objectSubscriber)->*(handler))();
        }
    
        void useHandlerWithData()
        {
            if(handlerWithData)
                ((objectSubscriber)->*(handlerWithData))(notificationReportedData);
        }
    
        naradaNotificationHandlerFunc getHandler()
        {
            return handler;
        }
    
        naradaNotificationHandlerFuncWithData getHandlerWithData()
        {
            return handlerWithData;
        }
    
};




/**
* @class NaradaNotificationCenter
* @brief Class that simplifies message notification between independent classes
*
*
*/
class VISAGE_DECLSPEC NaradaNotificationCenter
{

	public:
    

		NaradaNotificationCenter();
		virtual ~NaradaNotificationCenter();
    
        static void postNotification(std::string notification);
        static void postNotification(std::string notification, int data);
    
        static void subscribeNotification(std::string notification, void (*handler)());
        static void subscribeNotification(std::string notification, void (*handler)(int data));
    
        static void unsubscribeNotification(std::string notification, void (*handler)());
        static void unsubscribeNotification(std::string notification, void (*handler)(int data));
    
        static void unsubscribeAll();
    

    
	protected:
        static NaradaNotificationCenter *center;
        std::vector<NaradaNotificationRelationBase*> notificationsTable;


        static void checkInitCenter();
    
        static std::vector<NaradaNotificationRelationBase*> findActiveNotifications(std::string notification);
    
        static void removeNotificationRelation(std::string notification, void (*handler)());
        static void removeNotificationRelation(std::string notification, void (*handler)(int data));
    
        static void executeHandlers(std::vector<NaradaNotificationRelationBase*> pendingNotif);
    
        template <class NRSubscriber>   //implementation at the end of this document because of template restrictions
        static void removeNotificationRelation(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)());

        template <class NRSubscriber>   //implementation at the end of this document because of template restrictions
        static void removeNotificationRelation(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)(int data));
    
    
    public:
    
        template <class NRSubscriber>
        static void subscribeNotification(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)());
    
        template <class NRSubscriber>
        static void subscribeNotification(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)(int data));

        template <class NRSubscriber>
        static void unsubscribeNotification(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)());

        template <class NRSubscriber>
        static void unsubscribeNotification(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)(int data));

    
};

    
    
    
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    ////////////////// Template functions implementations //////////////////
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    
    
    /**
     * @brief Unsubscribe notification in case of C++ classes.
     * Deletes the relation between the method and the notification, so, when the notification is posted, the function passed as handler argument
     * won't be executed anymore
     *
     * @param *notification pointer to std::string with notification
     * @param NRSubscriber* template. Object class that owns the handler method
     * @param void (*handler)() pointer to a void method of the NRSubscriber template, that implements the handler for the notification
     *
     */
    template <class NRSubscriber>
    void NaradaNotificationCenter::removeNotificationRelation(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)())
    {
        for(int i=0; i<center->notificationsTable.size(); i++)
        {
            if(!notification.compare(center->notificationsTable.at(i)->getNotification()))
            {
                NaradaNotificationRelationCPPStyled<NRSubscriber> *storedSubscriber = dynamic_cast<NaradaNotificationRelationCPPStyled<NRSubscriber> *>(center->notificationsTable.at(i));
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
     * @brief Unsubscribe notification with data in case of C++ classes.
     * Deletes the relation between the method and the notification, so, when the notification is posted, the function passed as handler argument
     * won't be executed anymore
     *
     * @param *notification pointer to std::string with notification
     * @param NRSubscriber* template. Object class that owns the handler method
     * @param void (*handler)(int data) pointer to a void method with int argument of the NRSubscriber template, that implements the handler for the notification
     *
     */
    template <class NRSubscriber>
    void NaradaNotificationCenter::removeNotificationRelation(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)(int data))
    {
        for(int i=0; i<center->notificationsTable.size(); i++)
        {
            if(!notification.compare(center->notificationsTable.at(i)->getNotification()))
            {
                NaradaNotificationRelationCPPStyled<NRSubscriber> *storedSubscriber = dynamic_cast<NaradaNotificationRelationCPPStyled<NRSubscriber> *>(center->notificationsTable.at(i));
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
     * @brief Subscribe notification in case of C++ classes.
     * When the notification is posted, the method passed as handler argument will be executed
     *
     * @param *notification pointer to std::string with notification
     * @param NRSubscriber* template. Object class that owns the handler method
     * @param void (*handler)() pointer to a void method of the NRSubscriber template, that implements the handler for the notification
     *
     * Example usage:
     * @code
     *  class Foo
     *  {
     *      void subscriberFunction(){ std::cout << "Hello world!" << std::endl; }
     *      //...
     *
     *  //in the same class, for example:
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, this, &Foo::subscriberFunction);
     *
     *  //In another class, for example:...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber); //in another thread, this will write by console "Hello world!"
     *
     *  //When I want to cancel the subscription, in the Foo class, for example:
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, this, &Foo::subscriberFunction);
     *  @endcode
     *
     *
     */
    template <class NRSubscriber>
    void NaradaNotificationCenter::subscribeNotification(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)())
    {
        checkInitCenter();
        NaradaNotificationRelationCPPStyled<NRSubscriber> *notifRelation = new NaradaNotificationRelationCPPStyled<NRSubscriber>(notification, subscriber, handler);
        center->notificationsTable.push_back(notifRelation);
        std::cout << "subscrived handler for notification: " << notification.c_str() << std::endl;
    }



    /**
     * @brief Subscribe notification in case of C++ classes.
     * When the notification is posted, the method passed as handler argument will be executed. The handler must accept an int argument
     *
     * @param *notification pointer to std::string with notification
     * @param NRSubscriber* template. Object class that owns the handler method
     * @param void (*handler)(int data) pointer to a void method of the NRSubscriber template, that implements the handler for the notification
     *
     * Example usage:
     * @code
     *  class Foo
     *  {
     *      void subscriberFunction(int data){ for(int i=0; i<data; i++) {std::cout << "Hello world!" << std::endl;} }
     *      //...
     *
     *  //in the same class, for example:
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, this, &Foo::subscriberFunction);
     *
     *  //In another class, for example:...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber, 5); //in another thread, this will write by console "Hello world!" 5 times
     *
     *  //When I want to cancel the subscription, in the Foo class, for example:
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, this, &Foo::subscriberFunction);
     *  @endcode
     *
     *
     */
    template <class NRSubscriber>
    void NaradaNotificationCenter::subscribeNotification(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)(int data))
    {
        checkInitCenter();
        NaradaNotificationRelationCPPStyled<NRSubscriber> *notifRelation = new NaradaNotificationRelationCPPStyled<NRSubscriber>(notification, subscriber, handler);
        center->notificationsTable.push_back(notifRelation);
        std::cout << "subscrived handler for notification: " << notification.c_str() << std::endl;
    }




    /**
     * @brief Unsubscribe notification in case of C++ classes.
     * Deletes the relation between the method and the notification, so, when the notification is posted, the function passed as handler argument
     * won't be executed anymore
     *
     * @param *notification pointer to std::string with notification
     * @param NRSubscriber* template. Object class that owns the handler method
     * @param void (*handler)() pointer to a void method of the NRSubscriber template, that implements the handler for the notification
     *
     * Example usage:
     * @code
     *  class Foo
     *  {
     *      void subscriberFunction(){ std::cout << "Hello world!" << std::endl; }
     *      //...
     *
     *  //in the same class, for example:
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, this, &Foo::subscriberFunction);
     *
     *  //In another class, for example:...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber); //in another thread, this will write by console "Hello world!"
     *
     *  //When I want to cancel the subscription, in the Foo class, for example:
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, this, &Foo::subscriberFunction);
     *  @endcode
     *
     */
    template <class NRSubscriber>
    void NaradaNotificationCenter::unsubscribeNotification(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)())
    {
        checkInitCenter();
        //removeNotificationRelation(notification, *subscriber, handler);
        removeNotificationRelation(notification, subscriber, handler);
    }


    /**
     * @brief Unsubscribe notification in case of C++ classes.
     * Deletes the relation between the method and the notification, so, when the notification is posted, the function passed as handler argument
     * won't be executed anymore
     *
     * @param *notification pointer to std::string with notification
     * @param NRSubscriber* template. Object class that owns the handler method
     * @param void (*handler)(int data) pointer to a void method of the NRSubscriber template, that implements the handler for the notification
     *
     * Example usage:
     * @code
     *  class Foo
     *  {
     *      void subscriberFunction(int data){ for(int i=0; i<data; i++) {std::cout << "Hello world!" << std::endl;} }
     *      //...
     *
     *  //in the same class, for example:
     *  //add subscriber
     *  std::string notifySubscriber = "sayHelloWorld"
     *  NaradaNotificationCenter::subscribeNotification(notifySubscriber, this, &Foo::subscriberFunction);
     *
     *  //In another class, for example:...Where I want to launch the subscription
     *  NaradaNotificationCenter::postNotification(notifySubscriber, 5); //in another thread, this will write by console "Hello world!" 5 times
     *
     *  //When I want to cancel the subscription, in the Foo class, for example:
     *  NaradaNotificationCenter::unsubscribeNotification(notifySubscriber, this, &Foo::subscriberFunction);
     *  @endcode
     *
     */
    template <class NRSubscriber>
    void NaradaNotificationCenter::unsubscribeNotification(std::string notification, NRSubscriber *subscriber, void (NRSubscriber::*handler)(int data))
    {
        checkInitCenter();
        //removeNotificationRelation(notification, *subscriber, handler);
        removeNotificationRelation(notification, subscriber, handler);
    }




//}

#endif /* NARADA_NOTIFICATION_CENTER_H */

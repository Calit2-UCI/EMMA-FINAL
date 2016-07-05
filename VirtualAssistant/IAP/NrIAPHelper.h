//
//  NrIAPHelper.h
//  VirtualAssistant
//
//  Created by Enrique on 10/16/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

#define kProductsLoadedNotification         @"ProductsLoaded"
#define kProductPurchasedNotification       @"ProductPurchased"
#define kProductPurchaseFailedNotification  @"ProductPurchaseFailed"

@interface NrIAPHelper : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    NSSet * _productIdentifiers;
    NSArray * _products;
    NSMutableSet * _purchasedProducts;
    SKProductsRequest * _request;
}

@property (strong) NSSet * productIdentifiers;
@property (strong) NSArray * products;
@property (strong) NSMutableSet * purchasedProducts;
@property (strong) SKProductsRequest * request;

- (void)requestProducts;
- (id)initWithProductIdentifiers:(NSSet *)productIdentifiers;
- (void)buyProduct:(SKProduct *)productIdentifier;

@end

//
//  NaradaDownloader.h
//  NaradaDownloader
//
//  Created by Enrique on 5/15/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NaradaDownloaderDelegate.h"

@interface NaradaDownloader : NSObject <NSURLConnectionDelegate>

@property (nonatomic, strong) NSURLConnection *connection;
@property (nonatomic, strong) NSURL *url;
@property (nonatomic, strong) NSMutableData *receivedData;
@property (nonatomic, strong) NSString *receivedString;
@property (nonatomic, retain) id<NaradaDownloaderDelegate> delegate;

- (id)initWithURL:(NSURL *)url andDelegate:(id<NaradaDownloaderDelegate>)delegate;
- (id)initWithURLString:(NSString *)urlStr andDelegate:(id<NaradaDownloaderDelegate>)delegate;

- (void)requestDownload;

@end

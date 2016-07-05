//
//  NaradaDownloader.m
//  NaradaDownloader
//
//  Created by Enrique on 5/15/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import "NaradaDownloader.h"

@implementation NaradaDownloader

@synthesize connection = _connection;
@synthesize url = _url;
@synthesize receivedData = _receivedData;
@synthesize receivedString = _receivedString;

- (id)initWithURL:(NSURL *)url andDelegate:(id<NaradaDownloaderDelegate>)delegate
{
    if ((self = [super init])) {
        self.url = url;
        self.delegate = delegate;
    }

    return self;
}

- (id)initWithURLString:(NSString *)urlStr andDelegate:(id<NaradaDownloaderDelegate>)delegate
{
    if ((self = [super init])) {
        self.url = [NSURL URLWithString:urlStr];
        self.delegate = delegate;
    }
    
    return self;
}

- (void)requestDownload
{
    NSURLRequest *request = [NSURLRequest requestWithURL:self.url cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:60];
    
    NSURLConnection *theConnection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
    
    if (theConnection) {
        NSLog(@"Connected successfully");
        self.connection = theConnection;
        self.receivedData = [NSMutableData data];
        self.receivedString = [[NSString alloc] init];
    }
    else {
        NSLog(@"Connection failed");
    }
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    NSLog(@"Did receive response");
    [self.receivedData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    NSLog(@"Did receive data");
    [self.receivedData appendData:data];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    NSLog(@"Error in connection. Description: %@", [error localizedDescription]);
    
    [self.delegate naradaDownloader:self didFailWithError:error];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    NSLog(@"Succeded! Received %d bytes of data", [self.receivedData length]);
    
    self.receivedString = [[NSString alloc] initWithData:self.receivedData encoding:NSUTF8StringEncoding];
    
    [self.delegate naradaDownlaoderDidFinishDownloading:self];
}


@end

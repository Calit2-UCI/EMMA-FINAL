//
//  NaradaDownloaderDelegate.h
//  NaradaDownloader
//
//  Created by Enrique on 5/15/13.
//  Copyright (c) 2013 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>

@class NaradaDownloader;

@protocol NaradaDownloaderDelegate <NSObject>
- (void)naradaDownlaoderDidFinishDownloading:(NaradaDownloader *)downloader;
- (void)naradaDownloader:(NaradaDownloader *)downloader didFailWithError:(NSError *)error;
@end

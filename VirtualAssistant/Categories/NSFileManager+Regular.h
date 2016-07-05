//
//  NSFileManager+Regular.h
//  VirtualAssistant
//
//  Created by Enrique on 10/4/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSFileManager (Regular)

- (void)removeFiles:(NSRegularExpression*)regex inPath:(NSString*)path;
- (BOOL)fileExists:(NSRegularExpression*)regex inPath:(NSString*)path;
- (NSMutableArray *)filesMatching:(NSRegularExpression *)regex inPath:(NSString*)path;
+ (NSString *)documentsPath;
+ (NSString *)documentsURL;

@end

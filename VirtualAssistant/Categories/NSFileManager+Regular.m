//
//  NSFileManager+Regular.m
//  VirtualAssistant
//
//  Created by Enrique on 10/4/12.
//  Copyright (c) 2012 Narada Robotics S.L. All rights reserved.
//

#import "NSFileManager+Regular.h"

@implementation NSFileManager (Regular)

- (void)removeFiles:(NSRegularExpression*)regex inPath:(NSString*)path {
    NSDirectoryEnumerator *filesEnumerator = [self enumeratorAtPath:path];
    
    NSString *file;
    NSError *error;
    while (file = [filesEnumerator nextObject]) {
        NSUInteger match = [regex numberOfMatchesInString:file
                                                  options:0
                                                    range:NSMakeRange(0, [file length])];
        
        if (match) {
            [[NSFileManager defaultManager] removeItemAtPath:[path stringByAppendingPathComponent:file] error:&error];
        }
    }
}


- (BOOL)fileExists:(NSRegularExpression*)regex inPath:(NSString*)path
{
    NSDirectoryEnumerator *filesEnumerator = [self enumeratorAtPath:path];
    
    NSString *file;
    while (file = [filesEnumerator nextObject]) {
        NSUInteger match = [regex numberOfMatchesInString:file options:0 range:NSMakeRange(0, [file length])];
        if (match) {
            return YES;
        }
    }
    return NO;
}

- (NSMutableArray *)filesMatching:(NSRegularExpression *)regex inPath:(NSString*)path
{
    NSDirectoryEnumerator *filesEnumerator = [self enumeratorAtPath:path];
    
    NSString *file;
    NSMutableArray *array = [NSMutableArray array];
    while (file = [filesEnumerator nextObject]) {
        NSUInteger match = [regex numberOfMatchesInString:file options:0 range:NSMakeRange(0, [file length])];
        if (match) {
            [array addObject:file];
        }
    }
    return array;
}

- (NSMutableArray *)fileNamesMatching:(NSRegularExpression *)regex inPath:(NSString*)path
{
    NSDirectoryEnumerator *filesEnumerator = [self enumeratorAtPath:path];
    
    NSString *file;
    NSMutableArray *array = [NSMutableArray array];
    while (file = [filesEnumerator nextObject]) {
        NSUInteger match = [regex numberOfMatchesInString:file options:0 range:NSMakeRange(0, [file length])];
        if (match) {
            [array addObject:file];
        }
    }
    return array;
}

+ (NSString *)documentsPath
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    return basePath;
}

+ (NSString *)documentsURL
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    return [NSURL URLWithString:basePath];
}

@end

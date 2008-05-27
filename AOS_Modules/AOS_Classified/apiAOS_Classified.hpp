/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__apiAOS_Classified_HPP__ 
#define INCLUDED__apiAOS_Classified_HPP__

#ifdef AOS_CLASSIFIED_EXPORTS
#define AOS_CLASSIFIED_API __declspec(dllexport)
#else
#define AOS_CLASSIFIED_API __declspec(dllimport)
#endif

#endif // INCLUDED__apiAOS_Classified_HPP__

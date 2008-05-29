/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__apiAOS_Test_HPP__ 
#define INCLUDED__apiAOS_Test_HPP__

#ifdef AOS_TEST_EXPORTS
#define AOS_TEST_API __declspec(dllexport)
#else
#define AOS_TEST_API __declspec(dllimport)
#endif

#endif // INCLUDED__apiAOS_Test_HPP__

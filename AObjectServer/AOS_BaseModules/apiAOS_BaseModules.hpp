/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOS_BaseModules_HPP__
#define INCLUDED__AOS_BaseModules_HPP__

#ifdef AOS_BASEMODULES_EXPORTS
#define AOS_BASEMODULES_API __declspec(dllexport)
#else
#define AOS_BASEMODULES_API __declspec(dllimport)
#endif

#define ABase_INFO "AOS Base Modules 1.1.0.0 " ## ABASE_OS_INFO

#endif //INCLUDED__AOS_BaseModules_HPP__

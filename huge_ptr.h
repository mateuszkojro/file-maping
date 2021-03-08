#pragma once
#ifndef HUGE_PTR
#define HUGE_PTR
#define _X86_

#include <fileapi.h>
#include <windows.h>
#include <iostream>
#include <cassert>
#include <cstdio>
#include "allocator_exceptions.h"

// --- zaimplementowac jako alocator --
// https://en.cppreference.com/w/cpp/memory/allocator

typedef long long int s_size_t;

static size_t get_id() {
    static size_t id = 0;

    id++;
    return id;
}


namespace mk {

    template<class T>
    class huge_ptr {
    public:

        huge_ptr();

        huge_ptr(const huge_ptr &);

        huge_ptr(const huge_ptr &, s_size_t ofset);

        // dostep do koljenych adresow jak w zwyklym ptr
        T &operator[](size_t);

        // dereferenece the smart ptr
        T &operator*();

        // call a function on a ptr
        T *operator->();

        ~huge_ptr() {

            // CloseHandle(hFile) - can be used for closing handles
            // std::remove("temp_file"); - usuwam plik tymczasowy
            // UnmapViewOfFile(this->cur_ptr_); -- usuwam view

            this->free();

        }

        //const std::ptrdiff_t operator-(const huge_ptr& other) const;
        //const std::ptrdiff_t operator+(const huge_ptr& other) const;

        const huge_ptr &operator=(const huge_ptr &other);

        huge_ptr operator++();

        huge_ptr operator++(int);

        huge_ptr operator--();

        huge_ptr operator--(int);

        operator const bool() const;

        //reszta operatorow - casta na boola prownywanie itd.
        // tak jak np w unique_ptr

        static huge_ptr allocate_huge(size_t);

        static huge_ptr pointer_to(T &_Val) {

            throw "well thats shit";

            return allocate_huge(sizeof(_Val));
        }

        void free() {

            UnmapViewOfFile(cur_ptr_);
            UnmapViewOfFile(this->cur_ptr_);
            std::remove(generate_file_name());

        }

        bool operator==(const huge_ptr<T> ptr) const {

            return
                    maped_handle_ == ptr.maped_handle_
                    &&
                    ofset_ == ptr.ofset_;
        }

        bool operator!=(const huge_ptr<T> ptr) const {

            return !operator==(ptr);
        }

        friend const huge_ptr<T> operator+(huge_ptr<T> ptr, size_t ofset) {

            return mk::huge_ptr<T>(ptr, ptr.ofset_ + ofset);
        }

        friend const huge_ptr<T> operator-(huge_ptr<T> ptr, size_t ofset) {

            return mk::huge_ptr<T>(ptr, ptr.ofset_ - ofset);
        }

        friend const huge_ptr<T> operator+(huge_ptr<T> ptr, int ofset) {

            return mk::huge_ptr<T>(ptr, ptr.ofset_ + ofset);
        }

        friend const huge_ptr<T> operator-(huge_ptr<T> ptr, int ofset) {

            return mk::huge_ptr<T>(ptr, ptr.ofset_ - ofset);
        }

        friend const std::ptrdiff_t operator+(mk::huge_ptr<T> ptr1, mk::huge_ptr<T> ptr2) {

            return (ptr1.ofset_ + ptr2.ofset_);
        }

        friend const std::ptrdiff_t operator-(mk::huge_ptr<T> ptr1, mk::huge_ptr<T> ptr2) {

            return (ptr1.ofset_ - ptr2.ofset_);
        }


        char *generate_file_name() {

            char *string = new char[9];

            unsigned background = 10000000 + local_file_id_;


            string[0] = 'p';
            string[1] = 'f';

            for (int i = 2; i < 8; i++) {
                string[9 - i] = (char) ((background % 10) + 48);
                background /= 10;

            }
            string[8] = '\0';

            return string;
        }


        unsigned local_file_id_;


        // how far are we from the begining of the file
        size_t ofset_;


        // curent file view ptr
        T *cur_ptr_;

        HANDLE maped_handle_;


    };


}

// -------------------------------------
// implementations
// --------------------------------------


#include "huge_ptr.h"

namespace mk {


    // how much physicaal memory is left on the system
    static size_t get_memory_left() {
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullAvailPhys;
    }

    // we need to get the allocation granularity
    // na sczescie wystarczy pobraac @SYSTEM_INFO
    static size_t get_graniualrity() {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return info.dwAllocationGranularity;
    }

    template<class T>
    inline huge_ptr<T>::operator const bool() const {
        return maped_handle_;
    }

    // allocaate huge amounts of memory through file maping and creatin views into file
    // on disk
    /// @returns: big pointer

    template<class T>
    huge_ptr<T> huge_ptr<T>::allocate_huge(size_t alloc_size) {
        huge_ptr<T> temp;


        HANDLE file_handle =
                CreateFileA(
                        temp.generate_file_name(),            // file name
                        GENERIC_READ | GENERIC_WRITE,        // define desired acces r ights
                        FILE_SHARE_WRITE | FILE_SHARE_READ,    // can handle be shared NULL if not
                        NULL,                    //  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        CREATE_NEW,                //  DWORD       dwCreationDisposition,
                        FILE_ATTRIBUTE_NORMAL, //|
                        //FILE_FLAG_RANDOM_ACCESS,//FILE_FLAG_WRITE_THROUGH,			//  DWORD	dwFlagsAndAttributes,
                        NULL                    //  HANDLE      hTemplateFile
                );

        if (!file_handle) {
            std::cout << GetLastError() << std::endl;
            throw "empty_handle";
        }

        HANDLE maped_handle = CreateFileMappingA(
                file_handle,            // file handle from @CreateFileA
                NULL,                //LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
                PAGE_READWRITE,            //DWORD       flProtect,
                0,                //DWORD       dwMaximumSizeHigh,
                alloc_size,            // how much of memory to map
                NULL                // name for this handle if neded to share between processes
        );

        CloseHandle(file_handle);

        if (!maped_handle) {
            std::cout << GetLastError() << std::endl;
            throw "empty_handle";
        }

        LPVOID ptr = MapViewOfFile(
                maped_handle,    //HANDLE hFileMappingObject,
                FILE_MAP_ALL_ACCESS, //FILE_MAP_WRITE | FILE_MAP_READ,//	DWORD  dwDesiredAccess,
                0,    //DWORD  dwFileOffsetHigh,
                0,    //DWORD  dwFileOffsetLow,
                1    //SIZE_T dwNumberOfBytesToMap
        );

        if (!ptr) {
            throw "empty_view_ptr";
        }

        temp.maped_handle_ = maped_handle;
        temp.cur_ptr_ = (T *) ptr;
        temp.ofset_ = 0;

        return temp;
    }


    template<class T>
    huge_ptr<T>::huge_ptr() {
        local_file_id_ = get_id();
        cur_ptr_ = nullptr;
        ofset_ = 0;
    }

    template<class T>
    huge_ptr<T>::huge_ptr(const huge_ptr<T> &other) {
        local_file_id_ = other.local_file_id_;

        cur_ptr_ = other.cur_ptr_;
        ofset_ = other.ofset_;

        maped_handle_ = other.maped_handle_;
    }

    template<class T>
    const huge_ptr<T> &huge_ptr<T>::operator=(const huge_ptr<T> &other) {

        if (this == &other) return *this;
        local_file_id_ = other.local_file_id_;

        cur_ptr_ = other.cur_ptr_;
        ofset_ = other.ofset_;

        maped_handle_ = other.maped_handle_;
        return *this;

    }

    template<class T>
    huge_ptr<T>::huge_ptr(const huge_ptr<T> &other, s_size_t ofset) {

        local_file_id_ = other.local_file_id_;

        cur_ptr_ = other.cur_ptr_;
        ofset_ = other.ofset_ + ofset;

        maped_handle_ = other.maped_handle_;
    }

    // creates new file view from the desired position of size
    // equal to size of one stored element and return reference
    template<class T>
    T &huge_ptr<T>::operator[](size_t position) {

        const size_t graniularity = get_graniualrity();
        size_t calculated_position = position + ofset_;

        const size_t allocation_block = calculated_position / graniularity;


        calculated_position = calculated_position % graniularity;


        if (!this->maped_handle_) {
            throw "empty_handle";
        }
        UnmapViewOfFile(this->cur_ptr_);

        LPVOID ptr = MapViewOfFile(
                this->maped_handle_,                    //HANDLE hFileMappingObject,
                FILE_MAP_WRITE | FILE_MAP_READ,                //DWORD  dwDesiredAccess,
                0,                            //DWORD  dwFileOffsetHigh,
                graniularity * allocation_block,            //DWORD  dwFileOffsetLow,
                calculated_position + sizeof(T)         //SIZE_T dwNumberOfBytesToMap
        );

        if (!ptr) {
            std::cout << "err: " << GetLastError() << std::endl;
            throw "empty_view_ptr";

        }

        cur_ptr_ = (T *) ptr;

        return this->cur_ptr_[calculated_position / sizeof(T)];
    }

    template<class T>
    T &huge_ptr<T>::operator*() {
        return operator[](0);
    }

    template<class T>
    inline T *huge_ptr<T>::operator->() {

        const size_t graniularity = get_graniualrity();
        size_t calculated_position = ofset_;

        const size_t allocation_block = calculated_position / graniularity;
        calculated_position = calculated_position % graniularity;


        UnmapViewOfFile(this->cur_ptr_);

        LPVOID ptr = MapViewOfFile(
                this->maped_handle_,                    //HANDLE hFileMappingObject,
                FILE_MAP_WRITE | FILE_MAP_READ,                //DWORD  dwDesiredAccess,
                0,                            //DWORD  dwFileOffsetHigh,
                graniularity * allocation_block,            //DWORD  dwFileOffsetLow,
                calculated_position + sizeof(T)         //SIZE_T dwNumberOfBytesToMap
        );

        if (!ptr) {
            throw "empty_view_ptr";
        }

        cur_ptr_ = (T *) ptr;

        return this->cur_ptr_;

    }


    template<class T>
    huge_ptr<T> huge_ptr<T>::operator++() {
        return huge_ptr<T>(*this, ofset_ + 1);
    }

    template<class T>
    huge_ptr<T> huge_ptr<T>::operator++(int) {
        huge_ptr<T> temp = *this;
        operator++();
        return temp;
    }

    template<class T>
    huge_ptr<T> huge_ptr<T>::operator--() {
        return huge_ptr<T>(*this, ofset_ - 1);
    }

    template<class T>
    huge_ptr<T> huge_ptr<T>::operator--(int) {
        huge_ptr<T> temp = *this;
        operator--();
        return temp;
    }


}


#endif // HUGE_PTR

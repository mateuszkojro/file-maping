#pragma once
#ifndef HUGE_PTR
#define HUGE_PTR


#include <fileapi.h>
#include <windows.h>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <exception>

// --- zaimplementowac jako allocator --
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

        huge_ptr(const huge_ptr &, s_size_t offset);

        // dostep do koljenych adresow jak w zwyklym ptr
        T &operator[](size_t);

        // de-reference the smart ptr
        T &operator*();

        // call a function on a ptr
        T *operator->();

        ~huge_ptr() {

            // CloseHandle(hFile) - can be used for closing handles
            // std::remove("temp_file"); - usuwam plik tymczasowy
            // UnmapViewOfFile(this->cur_ptr_); -- usuwam view

            this->free();

        }



        const huge_ptr &operator=(const huge_ptr &other);

        huge_ptr operator++();

        const huge_ptr operator++(int);

        huge_ptr operator--();

        const huge_ptr operator--(int);

        operator bool() const;

        static huge_ptr allocate_huge(size_t);

        static huge_ptr pointer_to(T &_Val) {

            return allocate_huge(sizeof(_Val));
        }

        // free the memory
        void free() {

            UnmapViewOfFile(cur_ptr_);
            UnmapViewOfFile(this->cur_ptr_);
            std::remove(generate_file_name()); // usuwam plik

        }

        bool operator==(const huge_ptr<T> ptr) const {

            return
                    mapped_handle_ == ptr.mapped_handle_
                    &&
                    offset_ == ptr.offset_;
        }

        bool operator!=(const huge_ptr<T> ptr) const {

            return !operator==(ptr);
        }

        friend huge_ptr<T> operator+(huge_ptr<T> ptr, size_t offset) {

            return mk::huge_ptr<T>(ptr, ptr.offset_ + offset);
        }

        friend huge_ptr<T> operator-(huge_ptr<T> ptr, size_t offset) {

            return mk::huge_ptr<T>(ptr, ptr.offset_ - offset);
        }

        friend huge_ptr<T> operator+(huge_ptr<T> ptr, int offset) {

            return mk::huge_ptr<T>(ptr, ptr.offset_ + offset);
        }

        friend huge_ptr<T> operator-(huge_ptr<T> ptr, int offset) {

            return mk::huge_ptr<T>(ptr, ptr.offset_ - offset);
        }

        friend std::ptrdiff_t operator+(mk::huge_ptr<T> ptr1, mk::huge_ptr<T> ptr2) {

            return (ptr1.offset_ + ptr2.offset_);
        }

        friend std::ptrdiff_t operator-(mk::huge_ptr<T> ptr1, mk::huge_ptr<T> ptr2) {

            return (ptr1.offset_ - ptr2.offset_);
        }


        char *generate_file_name(); // creates name of the file from current local_file_id_

    protected:
        unsigned local_file_id_; // nr that will be visible in the name of file

        // how far are we from the begining of the file
        size_t offset_;

        // current file view ptr
        T *cur_ptr_;

        HANDLE mapped_handle_;

    };

}

// -------------------------------------
// implementations
// --------------------------------------

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
    static size_t get_granularity() {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return info.dwAllocationGranularity;
    }

    template<class T>
    inline huge_ptr<T>::operator bool() const {
        return mapped_handle_;
    }

    // allocate huge amounts of memory through file mapping and creating views into file
    // on disk
    /// @returns: big pointer

    template<class T>
    huge_ptr<T> huge_ptr<T>::allocate_huge(size_t alloc_size) {
        huge_ptr<T> temp;


        HANDLE file_handle =
                CreateFileA(
                        temp.generate_file_name(),                      // file name
                        GENERIC_READ | GENERIC_WRITE,                   // define desired access rights
                        FILE_SHARE_WRITE | FILE_SHARE_READ,             // can handle be shared NULL if not
                        NULL,                                           //  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                        CREATE_NEW,                                     //  DWORD       dwCreationDisposition,
                        FILE_ATTRIBUTE_NORMAL |
                        FILE_FLAG_WRITE_THROUGH,//FILE_FLAG_RANDOM_ACCESS,//FILE_FLAG_WRITE_THROUGH,			//  DWORD	dwFlagsAndAttributes,
                        NULL                                            //  HANDLE      hTemplateFile
                );

        if (!file_handle) {
            throw std::bad_alloc();
        }

        HANDLE mapped_handle = CreateFileMappingA(
                file_handle,                            // file handle from @CreateFileA
                nullptr,              //LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
                PAGE_READWRITE,                         //DWORD       flProtect,
                0,                     //DWORD       dwMaximumSizeHigh,
                alloc_size,                             // how much of memory to map
                nullptr                          // name for this handle if neded to share between processes
        );

        CloseHandle(file_handle);

        if (!mapped_handle) {

            // this may be thrown due to Insufficient system resources exist to complete the requested service.
            //int that case GetLastError() = 1450
            // try:


            std::cout<<GetLastError();
            throw std::bad_alloc();

        }

        LPVOID ptr = MapViewOfFile(
                mapped_handle,                        //HANDLE hFileMappingObject,
                FILE_MAP_ALL_ACCESS,    //FILE_MAP_WRITE | FILE_MAP_READ,   //	DWORD  dwDesiredAccess,
                0,                      //DWORD  dwFileOffsetHigh,
                0,                      //DWORD  dwFileOffsetLow,
                1                 //SIZE_T dwNumberOfBytesToMap
        );

        if (!ptr) {
            throw std::bad_alloc();
        }

        temp.mapped_handle_ = mapped_handle;
        temp.cur_ptr_ = (T *) ptr;
        temp.offset_ = 0;

        return temp;
    }


    template<class T>
    huge_ptr<T>::huge_ptr() {
        local_file_id_ = get_id();
        cur_ptr_ = nullptr;
        offset_ = 0;
    }

    template<class T>
    huge_ptr<T>::huge_ptr(const huge_ptr<T> &other) {
        local_file_id_ = other.local_file_id_;

        cur_ptr_ = other.cur_ptr_;
        offset_ = other.offset_;

        mapped_handle_ = other.mapped_handle_;
    }

    template<class T>
    const huge_ptr<T> &huge_ptr<T>::operator=(const huge_ptr<T> &other) {

        if (this == &other) return *this;
        local_file_id_ = other.local_file_id_;

        cur_ptr_ = other.cur_ptr_;
        offset_ = other.offset_;

        mapped_handle_ = other.mapped_handle_;
        return *this;

    }

    template<class T>
    huge_ptr<T>::huge_ptr(const huge_ptr<T> &other, s_size_t offset) {

        local_file_id_ = other.local_file_id_;

        cur_ptr_ = other.cur_ptr_;
        offset_ = other.offset_ + offset;

        mapped_handle_ = other.mapped_handle_;
    }

    // creates new file view from the desired position of size
    // equal to size of one stored element and return reference
    template<class T>
    T &huge_ptr<T>::operator[](size_t position) {

        const size_t granularity = get_granularity();
        size_t calculated_position = position + offset_;

        const size_t allocation_block = calculated_position / granularity;


        calculated_position = calculated_position % granularity;


        if (!this->mapped_handle_) {
            throw std::bad_alloc();
        }
        UnmapViewOfFile(this->cur_ptr_);

        LPVOID ptr = MapViewOfFile(
                this->mapped_handle_,                    //HANDLE hFileMappingObject,
                FILE_MAP_WRITE | FILE_MAP_READ,          //DWORD  dwDesiredaccesss,
                0,                                       //DWORD  dwFileOffsetHigh,
                granularity * allocation_block,          //DWORD  dwFileOffsetLow,
                calculated_position + sizeof(T)          //SIZE_T dwNumberOfBytesToMap
        );

        if (!ptr) {
            throw std::bad_alloc();

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

        const size_t granularity = get_granularity();
        size_t calculated_position = offset_;

        const size_t allocation_block = calculated_position / granularity;
        calculated_position = calculated_position % granularity;


        UnmapViewOfFile(this->cur_ptr_);

        LPVOID ptr = MapViewOfFile(
                this->mapped_handle_,                    //HANDLE hFileMappingObject,
                FILE_MAP_WRITE | FILE_MAP_READ,          //DWORD  dwDesiredaccesss,
                0,                                       //DWORD  dwFileOffsetHigh,
                granularity * allocation_block,          //DWORD  dwFileOffsetLow,
                calculated_position + sizeof(T)          //SIZE_T dwNumberOfBytesToMap
        );

        if (!ptr) {
            throw std::bad_alloc();
        }

        cur_ptr_ = (T *) ptr;

        return this->cur_ptr_;

    }


    template<class T>
    huge_ptr<T> huge_ptr<T>::operator++() {
        return huge_ptr<T>(*this, offset_ + 1);
    }

    template<class T>
    const huge_ptr<T> huge_ptr<T>::operator++(int) {
        huge_ptr<T> temp = *this;
        operator++();
        return temp;
    }

    template<class T>
    huge_ptr<T> huge_ptr<T>::operator--() {
        return huge_ptr<T>(*this, offset_ - 1);
    }

    template<class T>
    const huge_ptr<T> huge_ptr<T>::operator--(int) {
        huge_ptr<T> temp = *this;
        operator--();
        return temp;
    }

    template<class T>
    char *huge_ptr<T>::generate_file_name() {

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


}


#endif // HUGE_PTR

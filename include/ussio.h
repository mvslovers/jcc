/* See the end of this file for odd cases! */

#define __uss_accept(Socket_descriptor, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code) __uss_generic (508, 6, Socket_descriptor, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code)
#define __uss_accept_and_recv(Socket_desc, Accepted_socket, Remote_addr_len, Remote_addr, Local_addr_len, Local_addr, Buffer_len, Buffer, Buffer_alet, Return_value, Return_code, Reason_code) __uss_generic (1060, 12, \
                              Socket_desc, Accepted_socket, Remote_addr_len, Remote_addr, Local_addr_len, Local_addr, Buffer_len, Buffer, Buffer_alet, Return_value, Return_code, Reason_code)
#define __uss_access(Pathname_length, Pathname, Access_mode, Return_value, Return_code, Reason_code) __uss_generic (52, 6, Pathname_length, Pathname, Access_mode, Return_value, Return_code, Reason_code)
#define __uss_aio_suspend(Aiocb_Ptr_List, Aiocb_Ptr_Count, Seconds, Nanoseconds, Return_value, Return_code, Reason_code) __uss_generic (1088, 7, Aiocb_Ptr_List, Aiocb_Ptr_Count, Seconds, Nanoseconds, Return_value, Return_code, Reason_code)
#define __uss_alarm(Seconds, Return_value) __uss_generic (224, 2, Seconds, Return_value)
#define __uss_asyncio(Aiocb_len, Aiocb, Return_value, Return_code, Reason_code) __uss_generic (988, 5, Aiocb_len, Aiocb, Return_value, Return_code, Reason_code)
#define __uss_attach_exec(Pathname_length, Pathname, Argument_count, Argument_length_list, Argument_list, Environment_count, Environment_data_length, Environment_data_list, Exit_routine_address, Exit_parameter_list_address, \
                          Return_value, Return_code, Reason_code) __uss_generic (664, 13, Pathname_length, Pathname, Argument_count, Argument_length_list, Argument_list, Environment_count, Environment_data_length, \
                          Environment_data_list, Exit_routine_address, Exit_parameter_list_address, Return_value, Return_code, Reason_code)
#define __uss_attach_execmvs(Program_name_length, Program_name, Argument_length, Argument, Exit_routine_address, Exit_parameter_list_address, Return_value, Return_code, Reason_code) __uss_generic (668, 9, \
                             Program_name_length, Program_name, Argument_length, Argument, Exit_routine_address, Exit_parameter_list_address, Return_value, Return_code, Reason_code)
#define __uss_auth_check_rsrc_np(Cell_UUID, Principal_UUID, Userid_Length, Userid, Security_Class_Length, Security_Class, Entity_Name_Length, Entity_Name, Access_Type, Return_value, Return_code, Reason_code) __uss_generic (972, 12, \
                                 Cell_UUID, Principal_UUID, Userid_Length, Userid, Security_Class_Length, Security_Class, Entity_Name_Length, Entity_Name, Access_Type, Return_value, Return_code, Reason_code)
#define __uss_bind(Socket_descriptor, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code) __uss_generic (512, 6, Socket_descriptor, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code)
#define __uss_BPX1SEC(Function_code, Identity_type, Identity_length, Identity, Pass_length, Pass, Certificate_length, Certificate, Option_flags, Return_value, Return_code, Reason_code) __uss_generic (1044, 12, \
                      Function_code, Identity_type, Identity_length, Identity, Pass_length, Pass, Certificate_length, Certificate, Option_flags, Return_value, Return_code, Reason_code)
#define __uss_chattr(Pathname_length, Pathname, Attributes_length, Attributes, Return_value, Return_code, Reason_code) __uss_generic (500, 7, Pathname_length, Pathname, Attributes_length, Attributes, Return_value, Return_code, Reason_code)
#define __uss_chaudit(Pathname_length, Pathname, Audit_flags, Option_code, Return_value, Return_code, Reason_code) __uss_generic (84, 7, Pathname_length, Pathname, Audit_flags, Option_code, Return_value, Return_code, Reason_code)
#define __uss_chdir(Pathname_length, Pathname, Return_value, Return_code, Reason_code) __uss_generic (56, 5, Pathname_length, Pathname, Return_value, Return_code, Reason_code)
#define __uss_chmod(Pathname_length, Pathname, Mode, Return_value, Return_code, Reason_code) __uss_generic (60, 6, Pathname_length, Pathname, Mode, Return_value, Return_code, Reason_code)
#define __uss_chown(Pathname_length, Pathname, Owner_UID, Group_ID, Return_value, Return_code, Reason_code) __uss_generic (64, 7, Pathname_length, Pathname, Owner_UID, Group_ID, Return_value, Return_code, Reason_code)
#define __uss_chpriority(Which, Who, PriorityType, Priority, Return_value, Return_code, Reason_code) __uss_generic (764, 7, Which, Who, PriorityType, Priority, Return_value, Return_code, Reason_code)
#define __uss_chroot(Pathname_length, Pathname, Return_value, Return_code, Reason_code) __uss_generic (872, 5, Pathname_length, Pathname, Return_value, Return_code, Reason_code)
#define __uss_close(File_descriptor, Return_value, Return_code, Reason_code) __uss_generic (72, 4, File_descriptor, Return_value, Return_code, Reason_code)
#define __uss_closedir(Directory_file_descriptor, Return_value, Return_code, Reason_code) __uss_generic (68, 4, Directory_file_descriptor, Return_value, Return_code, Reason_code)
#define __uss_cond_cancel(Return_value, Return_code, Reason_code) __uss_generic (480, 3, Return_value, Return_code, Reason_code)
#define __uss_cond_post(Thread_ID, Event, Return_value, Return_code, Reason_code) __uss_generic (484, 5, Thread_ID, Event, Return_value, Return_code, Reason_code)
#define __uss_cond_setup(Event_list, Return_value, Return_code, Reason_code) __uss_generic (488, 4, Event_list, Return_value, Return_code, Reason_code)
#define __uss_cond_timed_wait(Seconds, Nanoseconds, Event_list, Seconds_remaining, Nanoseconds_remaining, Return_value, Return_code, Reason_code) __uss_generic (492, 8, \
                              Seconds, Nanoseconds, Event_list, Seconds_remaining, Nanoseconds_remaining, Return_value, Return_code, Reason_code)
#define __uss_cond_wait(Event_list, Return_value, Return_code, Reason_code) __uss_generic (496, 4, Event_list, Return_value, Return_code, Reason_code)
#define __uss_connect(Socket_descriptor, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code) __uss_generic (516, 6, Socket_descriptor, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code)
#define __uss_console_np(MsgAttributes_length, MsgAttributes, Modify_string_ptr, Modify_string_length, Console_command, Return_value, Return_code, Reason_code) __uss_generic (1012, 8, \
                         MsgAttributes_length, MsgAttributes, Modify_string_ptr, Modify_string_length, Console_command, Return_value, Return_code, Reason_code)
#define __uss_convert_id_np(Function_code, Principal_UUID, Cell_UUID, Userid_Length, Userid, Return_value, Return_code, Reason_code) __uss_generic (968, 8, \
                            Function_code, Principal_UUID, Cell_UUID, Userid_Length, Userid, Return_value, Return_code, Reason_code)
#define __uss___cpl(FunctionCode, Bufferlen, Buffer, Return_value, Return_code, Reason_code) __uss_generic (1132, 6, FunctionCode, Bufferlen, Buffer, Return_value, Return_code, Reason_code)
#define __uss_deletehfs(Entrypt_address, Return_value, Return_code, Reason_code) __uss_generic (888, 4, Entrypt_address, Return_value, Return_code, Reason_code)
#define __uss_exec(Pathname_length, Pathname, Argument_count, Argument_length_list, Argument_list, Environment_count, Environment_data_length, Environment_data_list, Exit_routine_address, Exit_parameter_list_address, Return_value, \
                   Return_code, Reason_code) __uss_generic (228, 13, Pathname_length, Pathname, Argument_count, Argument_length_list, Argument_list, Environment_count, Environment_data_length, Environment_data_list, Exit_routine_address, \
                   Exit_parameter_list_address, Return_value, Return_code, Reason_code)
#define __uss_execmvs(Program_name_length, Program_name, Argument_length, Argument, Exit_routine_address, Exit_parameter_list_address, Return_value, Return_code, Reason_code) __uss_generic (236, 9, \
                      Program_name_length, Program_name, Argument_length, Argument, Exit_routine_address, Exit_parameter_list_address, Return_value, Return_code, Reason_code)
#define __uss__exit(Status_field) __uss_generic (232, 1, Status_field)
#define __uss_extlink_np(Ext_name_length, Ext_name, Link_name_length, Link_name, Return_value, Return_code, Reason_code) __uss_generic (200, 7, Ext_name_length, Ext_name, Link_name_length, Link_name, Return_value, Return_code, Reason_code)
#define __uss_fchattr(File_descriptor, Attributes_length, Attributes, Return_value, Return_code, Reason_code) __uss_generic (504, 6, File_descriptor, Attributes_length, Attributes, Return_value, Return_code, Reason_code)
#define __uss_fchaudit(File_descriptor, Audit_flags, Option_code, Return_value, Return_code, Reason_code) __uss_generic (140, 6, File_descriptor, Audit_flags, Option_code, Return_value, Return_code, Reason_code)
#define __uss_fchdir(Directory_file_descriptor, Return_value, Return_code, Reason_code) __uss_generic (852, 4, Directory_file_descriptor, Return_value, Return_code, Reason_code)
#define __uss_fchmod(File_descriptor, Mode, Return_value, Return_code, Reason_code) __uss_generic (88, 5, File_descriptor, Mode, Return_value, Return_code, Reason_code)
#define __uss_fchown(File_descriptor, Owner_UID, Group_ID, Return_value, Return_code, Reason_code) __uss_generic (92, 6, File_descriptor, Owner_UID, Group_ID, Return_value, Return_code, Reason_code)
#define __uss_fcntl(File_descriptor, Action, Argument, Return_value, Return_code, Reason_code) __uss_generic (96, 6, File_descriptor, Action, Argument, Return_value, Return_code, Reason_code)
#define __uss_fork(Process_ID, Return_code, Reason_code) __uss_generic (240, 3, Process_ID, Return_code, Reason_code)
#define __uss_fpathconf(File_descriptor, Name, Return_value, Return_code, Reason_code) __uss_generic (100, 5, File_descriptor, Name, Return_value, Return_code, Reason_code)
#define __uss_FreeAddrInfo(Addr_Info_Ptr, Return_value, Return_code, Reason_code) __uss_generic (1168, 4, Addr_Info_Ptr, Return_value, Return_code, Reason_code)
#define __uss_fstat(File_descriptor, Status_area_length, Status_area, Return_value, Return_code, Reason_code) __uss_generic (104, 6, File_descriptor, Status_area_length, Status_area, Return_value, Return_code, Reason_code)
#define __uss_FstatVfs(File_descriptor, Status_area_length, Status_area, Return_value, Return_code, Reason_code) __uss_generic (848, 6, File_descriptor, Status_area_length, Status_area, Return_value, Return_code, Reason_code)
#define __uss_fsync(File_descriptor, Return_value, Return_code, Reason_code) __uss_generic (108, 4, File_descriptor, Return_value, Return_code, Reason_code)
#define __uss_ftruncate(File_descriptor, File_length, Return_value, Return_code, Reason_code) __uss_generic (112, 5, File_descriptor, File_length, Return_value, Return_code, Reason_code)
#define __uss_GetAddrInfo(Node_Name, Node_Name_Length, Service_Name, Service_Name_Length, Hints_Ptr, Results_Ptr, Canonical_Length, Return_value, Return_code, Reason_code) __uss_generic (1164, 10, \
                          Node_Name, Node_Name_Length, Service_Name, Service_Name_Length, Hints_Ptr, Results_Ptr, Canonical_Length, Return_value, Return_code, Reason_code)
#define __uss_getclientid(FunctionCode, Domain, Clientid, Return_value, Return_code, Reason_code) __uss_generic (1024, 6, FunctionCode, Domain, Clientid, Return_value, Return_code, Reason_code)
#define __uss_getcwd(Buffer_length, Buffer, Return_value, Return_code, Reason_code) __uss_generic (116, 5, Buffer_length, Buffer, Return_value, Return_code, Reason_code)
#define __uss_getegid(Effective_group_ID) __uss_generic (244, 1, Effective_group_ID)
#define __uss_geteuid(Effective_user_ID) __uss_generic (248, 1, Effective_user_ID)
#define __uss_getgid(Real_group_ID) __uss_generic (264, 1, Real_group_ID)
#define __uss_getgrent(Return_value, Return_code, Reason_code) __uss_generic (772, 3, Return_value, Return_code, Reason_code)
#define __uss_getgrgid(Group_ID, Return_value, Return_code, Reason_code) __uss_generic (252, 4, Group_ID, Return_value, Return_code, Reason_code)
#define __uss_getgrnam(Group_name_length, Group_name, Return_value, Return_code, Reason_code) __uss_generic (256, 5, Group_name_length, Group_name, Return_value, Return_code, Reason_code)
#define __uss_getgroups(Group_ID_list_size, Group_ID_list_pointer_address, Number_of_group_IDs, Return_code, Reason_code) __uss_generic (260, 5, Group_ID_list_size, Group_ID_list_pointer_address, Number_of_group_IDs, Return_code, Reason_code)
#define __uss_gethostbyaddr(Address, Address_length, Hostent_ptr, Domain, Return_value, Return_code, Reason_code) __uss_generic (1160, 7, Address, Address_length, Hostent_ptr, Domain, Return_value, Return_code, Reason_code)
#define __uss_gethostbyname(Name, Name_length, Hostent_ptr, Return_value, Return_code, Reason_code) __uss_generic (1156, 6, Name, Name_length, Hostent_ptr, Return_value, Return_code, Reason_code)
#define __uss_gethostid(Domain, Name_length, Name, Return_value, Return_code, Reason_code) __uss_generic (520, 6, Domain, Name_length, Name, Return_value, Return_code, Reason_code)
#define __uss_getitimer(Interval_Type, Interval_Value_Adr, Return_value, Return_code, Reason_code) __uss_generic (752, 5, Interval_Type, Interval_Value_Adr, Return_value, Return_code, Reason_code)
#define __uss_getlogin(Return_value) __uss_generic (268, 1, Return_value)
#define __uss_GetNameInfo(SockAddr, SockAddr_Length, Service_Buffer, Service_Buffer_Length, Host_Buffer, Host_Buffer_Length, Flags, Return_value, Return_code, Reason_code) __uss_generic (1172, 10, \
                          SockAddr, SockAddr_Length, Service_Buffer, Service_Buffer_Length, Host_Buffer, Host_Buffer_Length, Flags, Return_value, Return_code, Reason_code)
#define __uss_getpeername(Socket_descriptor, Operation, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code) __uss_generic (524, 7, Socket_descriptor, Operation, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code)
#define __uss_getpgid(PID, Return_value, Return_code, Reason_code) __uss_generic (860, 4, PID, Return_value, Return_code, Reason_code)
#define __uss_getpgrp(Group_ID) __uss_generic (272, 1, Group_ID)
#define __uss_getpid(Process_ID) __uss_generic (276, 1, Process_ID)
#define __uss_getppid(Return_value) __uss_generic (284, 1, Return_value)
#define __uss_getpriority(Which, Who, Return_value, Return_code, Reason_code) __uss_generic (744, 5, Which, Who, Return_value, Return_code, Reason_code)
#define __uss_getpwent(Return_value, Return_code, Reason_code) __uss_generic (776, 3, Return_value, Return_code, Reason_code)
#define __uss_getpwnam(User_name_length, User_name, Return_value, Return_code, Reason_code) __uss_generic (280, 5, User_name_length, User_name, Return_value, Return_code, Reason_code)
#define __uss_getpwuid(User_ID, Return_value, Return_code, Reason_code) __uss_generic (288, 4, User_ID, Return_value, Return_code, Reason_code)
#define __uss_getrlimit(Resource, Rlimit, Return_value, Return_code, Reason_code) __uss_generic (820, 5, Resource, Rlimit, Return_value, Return_code, Reason_code)
#define __uss_getrusage(Who, Rusage, Return_value, Return_code, Reason_code) __uss_generic (824, 5, Who, Rusage, Return_value, Return_code, Reason_code)
#define __uss_getsid(PID, Return_value, Return_code, Reason_code) __uss_generic (864, 4, PID, Return_value, Return_code, Reason_code)
#define __uss_getsockname(Socket_descriptor, Operation, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code) __uss_generic (524, 7, Socket_descriptor, Operation, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code)
#define __uss_getsockopt(Socket_descriptor, Operation, Level, Option_name, Option_data_length, Option_data, Return_value, Return_code, Reason_code) __uss_generic (528, 9, \
                         Socket_descriptor, Operation, Level, Option_name, Option_data_length, Option_data, Return_value, Return_code, Reason_code)
#define __uss___getthent(Input_length, Input_address, Output_length, Output_address, Return_value, Return_code, Reason_code) __uss_generic (1056, 7, Input_length, Input_address, Output_length, Output_address, Return_value, Return_code, Reason_code)
#define __uss_getuid(User_ID) __uss_generic (296, 1, User_ID)
#define __uss_getwd(Buffer_length, Buffer, Return_value, Return_code, Reason_code) __uss_generic (936, 5, Buffer_length, Buffer, Return_value, Return_code, Reason_code)
#define __uss_givesocket(Socket_descriptor, Clientid, Return_value, Return_code, Reason_code) __uss_generic (1028, 5, Socket_descriptor, Clientid, Return_value, Return_code, Reason_code)
#define __uss_grantpt(File_descriptor, Return_value, Return_code, Reason_code) __uss_generic (916, 4, File_descriptor, Return_value, Return_code, Reason_code)
#define __uss_isatty(File_descriptor, Return_value) __uss_generic (12, 2, File_descriptor, Return_value)
#define __uss_isatty2(File_descriptor, Return_value, Return_code, Reason_code) __uss_generic (928, 4, File_descriptor, Return_value, Return_code, Reason_code)
#define __uss_kill(Process_ID, Signal, Signal_Options, Return_value, Return_code, Reason_code) __uss_generic (308, 6, Process_ID, Signal, Signal_Options, Return_value, Return_code, Reason_code)
#define __uss_lchattr(Pathname_length, Pathname, Attributes_length, Attributes, Return_value, Return_code, Reason_code) __uss_generic (1180, 7, Pathname_length, Pathname, Attributes_length, Attributes, Return_value, Return_code, Reason_code)
#define __uss_lchown(Pathname_length, Pathname, Owner_UID, Group_ID, Return_value, Return_code, Reason_code) __uss_generic (832, 7, Pathname_length, Pathname, Owner_UID, Group_ID, Return_value, Return_code, Reason_code)
#define __uss_link(Filename_length, Filename, Link_name_length, Link_name, Return_value, Return_code, Reason_code) __uss_generic (124, 7, Filename_length, Filename, Link_name_length, Link_name, Return_value, Return_code, Reason_code)
#define __uss_listen(Socket_descriptor, Backlog, Return_value, Return_code, Reason_code) __uss_generic (532, 5, Socket_descriptor, Backlog, Return_value, Return_code, Reason_code)
#define __uss_loadhfs(Filename_length, Filename, Flags, Libpath_length, Libpath, Return_value, Return_code, Reason_code) __uss_generic (880, 8, Filename_length, Filename, Flags, Libpath_length, Libpath, Return_value, Return_code, Reason_code)
#define __uss_lseek(File_descriptor, Offset, Reference_point, Return_value, Return_code, Reason_code) __uss_generic (128, 6, File_descriptor, Offset, Reference_point, Return_value, Return_code, Reason_code)
#define __uss_lstat(Pathname_length, Pathname, Status_area_length, Status_area, Return_value, Return_code, Reason_code) __uss_generic (132, 7, Pathname_length, Pathname, Status_area_length, Status_area, Return_value, Return_code, Reason_code)
#define __uss___map_init(FunctionCode, ParmListPtr, Return_value, Return_code, Reason_code) __uss_generic (1136, 5, FunctionCode, ParmListPtr, Return_value, Return_code, Reason_code)
#define __uss___map_service(FunctionCode, ParmListPtr, ArrayCount, Return_value, Return_code, Reason_code) __uss_generic (1140, 6, FunctionCode, ParmListPtr, ArrayCount, Return_value, Return_code, Reason_code)
#define __uss_mkdir(Pathname_length, Pathname, Mode, Return_value, Return_code, Reason_code) __uss_generic (136, 6, Pathname_length, Pathname, Mode, Return_value, Return_code, Reason_code)
#define __uss_mknod(Pathname_length, Pathname, Mode, Device_identifier, Return_value, Return_code, Reason_code) __uss_generic (144, 7, Pathname_length, Pathname, Mode, Device_identifier, Return_value, Return_code, Reason_code)
#define __uss_mmap(Map_address, Map_length, Protect_options, Map_type, File_descriptor, File_offset, Return_value, Return_code, Reason_code) __uss_generic (796, 9, \
                   Map_address, Map_length, Protect_options, Map_type, File_descriptor, File_offset, Return_value, Return_code, Reason_code)
#define __uss_mount(MountPoint_length, MountPoint_name, File_system_name, File_system_type, Mount_mode, Parm_length, Parm, Return_value, Return_code, Reason_code) __uss_generic (148, 10, \
                    MountPoint_length, MountPoint_name, File_system_name, File_system_type, Mount_mode, Parm_length, Parm, Return_value, Return_code, Reason_code)
#define __uss___mount(Mnte_length, Mnte, Return_value, Return_code, Reason_code) __uss_generic (1128, 5, Mnte_length, Mnte, Return_value, Return_code, Reason_code)
#define __uss_mprotect(Map_address, Map_length, Protect_options, Return_value, Return_code, Reason_code) __uss_generic (800, 6, Map_address, Map_length, Protect_options, Return_value, Return_code, Reason_code)
#define __uss_msgctl(Message_Queue_ID, Command, Buffer, Return_value, Return_code, Reason_code) __uss_generic (692, 6, Message_Queue_ID, Command, Buffer, Return_value, Return_code, Reason_code)
#define __uss_msgget(Key, Message_Flag, Return_value, Return_code, Reason_code) __uss_generic (696, 5, Key, Message_Flag, Return_value, Return_code, Reason_code)
#define __uss_msgrcv(Message_Queue_ID, Message_Address, Message_Alet, Message_Length, Message_Type, Message_Flag, Return_value, Return_code, Reason_code) __uss_generic (700, 9, \
                     Message_Queue_ID, Message_Address, Message_Alet, Message_Length, Message_Type, Message_Flag, Return_value, Return_code, Reason_code)
#define __uss_msgsnd(Message_Queue_ID, Message_address, Message_Alet, Message_Size, Message_Flag, Return_value, Return_code, Reason_code) __uss_generic (704, 8, \
                     Message_Queue_ID, Message_address, Message_Alet, Message_Size, Message_Flag, Return_value, Return_code, Reason_code)
#define __uss_msync(Map_address, Map_length, Sync_Options, Return_value, Return_code, Reason_code) __uss_generic (804, 6, Map_address, Map_length, Sync_Options, Return_value, Return_code, Reason_code)
#define __uss_munmap(Map_address, Map_length, Return_value, Return_code, Reason_code) __uss_generic (808, 5, Map_address, Map_length, Return_value, Return_code, Reason_code)
#define __uss_MvsIptAffinity(Routine_address, Parameter_list, Return_value, Return_code, Reason_code) __uss_generic (396, 5, Routine_address, Parameter_list, Return_value, Return_code, Reason_code)
#define __uss_MVSpause(Return_value, Return_code, Reason_code) __uss_generic (688, 3, Return_value, Return_code, Reason_code)
#define __uss_MVSpauseInit(Addr_of_ECBlist, Return_value, Return_code, Reason_code) __uss_generic (680, 4, Addr_of_ECBlist, Return_value, Return_code, Reason_code)
#define __uss_mvsprocclp(Status_field, Return_value, Return_code, Reason_code) __uss_generic (408, 4, Status_field, Return_value, Return_code, Reason_code)
#define __uss_mvssigsetup(Signal_interface_routine_address, User_data, Default_override_signal_set, Default_terminate_signal_set, Return_value, Return_code, Reason_code) __uss_generic (312, 7, \
                          Signal_interface_routine_address, User_data, Default_override_signal_set, Default_terminate_signal_set, Return_value, Return_code, Reason_code)
#define __uss_MvsThreadAffinity(Routine_address, Parameter_list, Thread_ID, Return_value, Return_code, Reason_code) __uss_generic (1148, 6, Routine_address, Parameter_list, Thread_ID, Return_value, Return_code, Reason_code)
#define __uss_mvsunsigsetup(Signal_interface_routine_address, User_data, Default_override_signal_set, Default_terminate_signal_set, Return_value, Return_code, Reason_code) __uss_generic (336, 7, \
                            Signal_interface_routine_address, User_data, Default_override_signal_set, Default_terminate_signal_set, Return_value, Return_code, Reason_code)
#define __uss_nice(Nice_change, Return_value, Return_code, Reason_code) __uss_generic (748, 4, Nice_change, Return_value, Return_code, Reason_code)
#define __uss_oe_env_np(Function_code, InArgCount, InArgListPtr, OutArgCount, OutArgListPtr, Return_value, Return_code, Reason_code) __uss_generic (960, 8, \
                        Function_code, InArgCount, InArgListPtr, OutArgCount, OutArgListPtr, Return_value, Return_code, Reason_code)
#define __uss_open(Pathname_length, Pathname, Options, Mode, Return_value, Return_code, Reason_code) __uss_generic (156, 7, Pathname_length, Pathname, Options, Mode, Return_value, Return_code, Reason_code)
#define __uss_opendir(Directory_name_length, Directory_name, Return_value, Return_code, Reason_code) __uss_generic (152, 5, Directory_name_length, Directory_name, Return_value, Return_code, Reason_code)
#define __uss_openstat(Pathname_length, Pathname, Options, Mode, Status_area_length, Status_area, Return_value, Return_code, Reason_code) __uss_generic (1052, 9, \
                       Pathname_length, Pathname, Options, Mode, Status_area_length, Status_area, Return_value, Return_code, Reason_code)
#define __uss___osenv(Function_code, Request_flags, Osenv_token, Return_value, Return_code, Reason_code) __uss_generic (1100, 6, Function_code, Request_flags, Osenv_token, Return_value, Return_code, Reason_code)
#define __uss___passwd(User_name_length, User_name, Password_length, Password, New_Password_length, New_Password, Return_value, Return_code, Reason_code) __uss_generic (788, 9, \
                       User_name_length, User_name, Password_length, Password, New_Password_length, New_Password, Return_value, Return_code, Reason_code)
#define __uss_pathconf(Pathname_length, Pathname, Name, Return_value, Return_code, Reason_code) __uss_generic (160, 6, Pathname_length, Pathname, Name, Return_value, Return_code, Reason_code)
#define __uss_pause(Return_value, Return_code, Reason_code) __uss_generic (316, 3, Return_value, Return_code, Reason_code)
#define __uss_pfsctl(File_System_type, Command, Argument_length, Argument, Return_value, Return_code, Reason_code) __uss_generic (768, 7, File_System_type, Command, Argument_length, Argument, Return_value, Return_code, Reason_code)
#define __uss___pid_affinity(Function_code, Target_Pid, Signal_Pid, Signal, Return_value, Return_code, Reason_code) __uss_generic (1072, 7, Function_code, Target_Pid, Signal_Pid, Signal, Return_value, Return_code, Reason_code)
#define __uss_pipe(Read_file_descriptor, Write_file_descriptor, Return_value, Return_code, Reason_code) __uss_generic (164, 5, Read_file_descriptor, Write_file_descriptor, Return_value, Return_code, Reason_code)
#define __uss___poe(Poecb_length, Poecb, Return_value, Return_code, Reason_code) __uss_generic (1176, 5, Poecb_length, Poecb, Return_value, Return_code, Reason_code)
#define __uss_poll(PollArrayPtr, NMsgsFds, Timeout, Return_value, Return_code, Reason_code) __uss_generic (932, 6, PollArrayPtr, NMsgsFds, Timeout, Return_value, Return_code, Reason_code)
#define __uss_Pread(File_descriptor, Fuio_Address, Fuio_Alet, Fuio_Length, Return_value, Return_code, Reason_code) __uss_generic (1108, 7, File_descriptor, Fuio_Address, Fuio_Alet, Fuio_Length, Return_value, Return_code, Reason_code)
#define __uss_Pwrite(File_descriptor, Fuio_Address, Fuio_Alet, Fuio_Length, Return_value, Return_code, Reason_code) __uss_generic (1108, 7, File_descriptor, Fuio_Address, Fuio_Alet, Fuio_Length, Return_value, Return_code, Reason_code)
#define __uss_pthread_cancel(Thread_ID, Return_value, Return_code, Reason_code) __uss_generic (448, 4, Thread_ID, Return_value, Return_code, Reason_code)
#define __uss_pthread_create(Init_rtn_addr, Work_area_addr, Attribute_area_addr, Thread_ID, Return_value, Return_code, Reason_code) __uss_generic (432, 7, \
                             Init_rtn_addr, Work_area_addr, Attribute_area_addr, Thread_ID, Return_value, Return_code, Reason_code)
#define __uss_pthread_detach(Thread_ID, Return_value, Return_code, Reason_code) __uss_generic (444, 4, Thread_ID, Return_value, Return_code, Reason_code)
#define __uss_pthread_join(Thread_ID, Status_field_address, Return_value, Return_code, Reason_code) __uss_generic (440, 5, Thread_ID, Status_field_address, Return_value, Return_code, Reason_code)
#define __uss_pthread_kill(Thread_ID, Signal, Signal_options, Return_value, Return_code, Reason_code) __uss_generic (464, 6, Thread_ID, Signal, Signal_options, Return_value, Return_code, Reason_code)
#define __uss_pthread_quiesc(Quiesce_type, User_data, Return_value, Return_code, Reason_code) __uss_generic (412, 5, Quiesce_type, User_data, Return_value, Return_code, Reason_code)
#define __uss_pthread_quiesce_and_get_np(RequestType, ThdQDataList, UserData, Return_value, Return_code, Reason_code) __uss_generic (1152, 6, RequestType, ThdQDataList, UserData, Return_value, Return_code, Reason_code)
#define __uss_pthread_self(Thread_ID) __uss_generic (452, 1, Thread_ID)
#define __uss_pthread_setintr(Interrupt_state, Return_value, Return_code, Reason_code) __uss_generic (460, 4, Interrupt_state, Return_value, Return_code, Reason_code)
#define __uss_pthread_setintrtype(Interrupt_type, Return_value, Return_code, Reason_code) __uss_generic (472, 4, Interrupt_type, Return_value, Return_code, Reason_code)
#define __uss_pthread_tag_np(New_Tag_Length, New_Tag_Ptr, Old_Tag_Length, Old_Tag_Ptr, Return_value, Return_code, Reason_code) __uss_generic (1016, 7, New_Tag_Length, New_Tag_Ptr, Old_Tag_Length, Old_Tag_Ptr, Return_value, Return_code, Reason_code)
#define __uss_pthread_testintr(Return_value, Return_code, Reason_code) __uss_generic (476, 3, Return_value, Return_code, Reason_code)
#define __uss_ptrace(Request, Process, Address, Data, Buffer, Return_value, Return_code, Reason_code) __uss_generic (320, 8, Request, Process, Address, Data, Buffer, Return_value, Return_code, Reason_code)
#define __uss_querydub(Return_value, Return_code, Reason_code) __uss_generic (948, 3, Return_value, Return_code, Reason_code)
#define __uss_quiesce(File_system_name, Return_value, Return_code, Reason_code) __uss_generic (388, 4, File_system_name, Return_value, Return_code, Reason_code)
#define __uss_read(File_descriptor, Buffer_address, Buffer_ALET, Read_count, Return_value, Return_code, Reason_code) __uss_generic (176, 7, File_descriptor, Buffer_address, Buffer_ALET, Read_count, Return_value, Return_code, Reason_code)
#define __uss_readdir(Directory_file_descriptor, Buffer_address, Buffer_ALET, Buffer_length, Return_value, Return_code, Reason_code) __uss_generic (168, 7, \
                      Directory_file_descriptor, Buffer_address, Buffer_ALET, Buffer_length, Return_value, Return_code, Reason_code)
#define __uss_readdir2(Directory_file_descriptor, UIO, Return_value, Return_code, Reason_code) __uss_generic (856, 5, Directory_file_descriptor, UIO, Return_value, Return_code, Reason_code)
#define __uss_read_extlink(Link_name_length, Link_name, Buffer_length, Buffer_address, Return_value, Return_code, Reason_code) __uss_generic (940, 7, Link_name_length, Link_name, Buffer_length, Buffer_address, Return_value, Return_code, Reason_code)
#define __uss_readlink(Link_name_length, Link_name, Buffer_length, Buffer_address, Return_value, Return_code, Reason_code) __uss_generic (172, 7, Link_name_length, Link_name, Buffer_length, Buffer_address, Return_value, Return_code, Reason_code)
#define __uss_readv(File_descriptor, Iov_count, Iov_struct, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code) __uss_generic (536, 8, \
                    File_descriptor, Iov_count, Iov_struct, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code)
#define __uss_realpath(Pathname_length, Pathname, Resolved_name_length, Resolved_name, Return_value, Return_code, Reason_code) __uss_generic (884, 7, \
                       Pathname_length, Pathname, Resolved_name_length, Resolved_name, Return_value, Return_code, Reason_code)
#define __uss_recv(Socket_descriptor, Buffer_length, Buffer, Buffer_alet, Flags, Return_value, Return_code, Reason_code) __uss_generic (540, 8, Socket_descriptor, Buffer_length, Buffer, Buffer_alet, Flags, Return_value, Return_code, Reason_code)
#define __uss_recvfrom(Socket_descriptor, Buffer_length, Buffer, Buffer_alet, Flags, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code) __uss_generic (544, 10, \
                       Socket_descriptor, Buffer_length, Buffer, Buffer_alet, Flags, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code)
#define __uss_recvmsg(Socket_descriptor, Message_hdr, Flags, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code) __uss_generic (548, 8, \
                      Socket_descriptor, Message_hdr, Flags, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code)
#define __uss_recvmsg2(Socket_descriptor, Message_hdr, Flags, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code) __uss_generic (976, 8, \
                       Socket_descriptor, Message_hdr, Flags, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code)
#define __uss_rename(Old_name_length, Old_name, New_name_length, New_name, Return_value, Return_code, Reason_code) __uss_generic (180, 7, Old_name_length, Old_name, New_name_length, New_name, Return_value, Return_code, Reason_code)
#define __uss_resource(Data_area_length, Data_area, Return_value, Return_code, Reason_code) __uss_generic (8, 5, Data_area_length, Data_area, Return_value, Return_code, Reason_code)
#define __uss_rewinddir(Directory_file_descriptor, Return_value, Return_code, Reason_code) __uss_generic (184, 4, Directory_file_descriptor, Return_value, Return_code, Reason_code)
#define __uss_rmdir(Directory_name_length, Directory_name, Return_value, Return_code, Reason_code) __uss_generic (188, 5, Directory_name_length, Directory_name, Return_value, Return_code, Reason_code)
#define __uss_select(Number_msgsfds, Read_list_length, Read_list, Write_list_length, Write_list, Exception_list_length, Exception_list, Timeout_pointer, Ecb_pointer, User_option_field, Return_value, Return_code, Reason_code) __uss_generic (552, 13, \
                     Number_msgsfds, Read_list_length, Read_list, Write_list_length, Write_list, Exception_list_length, Exception_list, Timeout_pointer, Ecb_pointer, User_option_field, Return_value, Return_code, Reason_code)
#define __uss_semget(Key, Number_of_Semaphores, Semaphore_Flags, Return_value, Return_code, Reason_code) __uss_generic (712, 6, Key, Number_of_Semaphores, Semaphore_Flags, Return_value, Return_code, Reason_code)
#define __uss_semop(Semaphore_ID, Semaphore_Operations, Number_of_Semaphore_Operations, Return_value, Return_code, Reason_code) __uss_generic (716, 6, \
                    Semaphore_ID, Semaphore_Operations, Number_of_Semaphore_Operations, Return_value, Return_code, Reason_code)
#define __uss_send(Socket_descriptor, Buffer_length, Buffer, Buffer_alet, Flags, Return_value, Return_code, Reason_code) __uss_generic (556, 8, Socket_descriptor, Buffer_length, Buffer, Buffer_alet, Flags, Return_value, Return_code, Reason_code)
#define __uss_sendmsg(Socket_descriptor, Message_hdr, Flags, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code) __uss_generic (560, 8, \
                      Socket_descriptor, Message_hdr, Flags, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code)
#define __uss_sendmsg2(Socket_descriptor, Message_hdr, Flags, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code) __uss_generic (980, 8, \
                       Socket_descriptor, Message_hdr, Flags, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code)
#define __uss_sendto(Socket_descriptor, Buffer_length, Buffer, Buffer_alet, Flags, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code) __uss_generic (564, 10, \
                     Socket_descriptor, Buffer_length, Buffer, Buffer_alet, Flags, Sockaddr_length, Sockaddr, Return_value, Return_code, Reason_code)
#define __uss_server_init(ManagerType, SubSystemType, SubSystemName, ApplEnv, ParallelEu, Return_value, Return_code, Reason_code) __uss_generic (1004, 8, \
                          ManagerType, SubSystemType, SubSystemName, ApplEnv, ParallelEu, Return_value, Return_code, Reason_code)
#define __uss_server_pwu(FcnCode, TransClass, ApplEnv, ClassifyAreaLen, ClassifyAreaPtr, ApplDataLen, ApplDataPtr, FdStrucPtr, Return_value, Return_code, Reason_code) __uss_generic (1008, 11, \
                         FcnCode, TransClass, ApplEnv, ClassifyAreaLen, ClassifyAreaPtr, ApplDataLen, ApplDataPtr, FdStrucPtr, Return_value, Return_code, Reason_code)
#define __uss_set_dub_default(Dub_setting, Return_value, Return_code, Reason_code) __uss_generic (300, 4, Dub_setting, Return_value, Return_code, Reason_code)
#define __uss_setegid(Group_ID, Return_value, Return_code, Reason_code) __uss_generic (424, 4, Group_ID, Return_value, Return_code, Reason_code)
#define __uss_seteuid(User_ID, Return_value, Return_code, Reason_code) __uss_generic (420, 4, User_ID, Return_value, Return_code, Reason_code)
#define __uss_setgid(Group_ID, Return_value, Return_code, Reason_code) __uss_generic (328, 4, Group_ID, Return_value, Return_code, Reason_code)
#define __uss_setgrent(Return_value) __uss_generic (780, 1, Return_value)
#define __uss_setgroups(SGid_list_count, SGid_list, Return_value, Return_code, Reason_code) __uss_generic (792, 5, SGid_list_count, SGid_list, Return_value, Return_code, Reason_code)
#define __uss_setitimer(Interval_Type, Interval_Value_Adr, Old_Interval_Value_Adr, Return_value, Return_code, Reason_code) __uss_generic (756, 6, Interval_Type, Interval_Value_Adr, Old_Interval_Value_Adr, Return_value, Return_code, Reason_code)
#define __uss_setpeer(Socket_descriptor, Sockaddr_length, Sockaddr, Option, Return_value, Return_code, Reason_code) __uss_generic (568, 7, Socket_descriptor, Sockaddr_length, Sockaddr, Option, Return_value, Return_code, Reason_code)
#define __uss_setpgid(Process_ID, Process_group_ID, Return_value, Return_code, Reason_code) __uss_generic (348, 5, Process_ID, Process_group_ID, Return_value, Return_code, Reason_code)
#define __uss_setpriority(Which, Who, Priority, Return_value, Return_code, Reason_code) __uss_generic (740, 6, Which, Who, Priority, Return_value, Return_code, Reason_code)
#define __uss_setpwent(Return_value) __uss_generic (784, 1, Return_value)
#define __uss_setregid(RGID, EGID, Return_value, Return_code, Reason_code) __uss_generic (896, 5, RGID, EGID, Return_value, Return_code, Reason_code)
#define __uss_setreuid(RUID, EUID, Return_value, Return_code, Reason_code) __uss_generic (892, 5, RUID, EUID, Return_value, Return_code, Reason_code)
#define __uss_setrlimit(Resource, Rlimit, Return_value, Return_code, Reason_code) __uss_generic (816, 5, Resource, Rlimit, Return_value, Return_code, Reason_code)
#define __uss_setsid(Session_ID, Return_code, Reason_code) __uss_generic (356, 3, Session_ID, Return_code, Reason_code)
#define __uss_setsockopt(Socket_descriptor, Operation, Level, Option_name, Option_data_length, Option_data, Return_value, Return_code, Reason_code) __uss_generic (528, 9, \
                         Socket_descriptor, Operation, Level, Option_name, Option_data_length, Option_data, Return_value, Return_code, Reason_code)
#define __uss_set_timer_event(Seconds, Nanoseconds, Return_value, Return_code, Reason_code) __uss_generic (1076, 5, Seconds, Nanoseconds, Return_value, Return_code, Reason_code)
#define __uss_setuid(User_ID, Return_value, Return_code, Reason_code) __uss_generic (364, 4, User_ID, Return_value, Return_code, Reason_code)
#define __uss_shmat(Shared_Memory_ID, Shared_Memory_Address, Shared_Memory_Flag, Return_value, Return_code, Reason_code) __uss_generic (720, 6, \
                    Shared_Memory_ID, Shared_Memory_Address, Shared_Memory_Flag, Return_value, Return_code, Reason_code)
#define __uss_shmctl(Shared_Memory_ID, Command, Buffer_address, Return_value, Return_code, Reason_code) __uss_generic (724, 6, Shared_Memory_ID, Command, Buffer_address, Return_value, Return_code, Reason_code)
#define __uss_shmdt(Shared_Memory_Address, Return_value, Return_code, Reason_code) __uss_generic (728, 4, Shared_Memory_Address, Return_value, Return_code, Reason_code)
#define __uss___shm_lock(LockFcnCode, LockReqType, LockType, LockAddr, LockAttrAddr, LockTokenAddr, Return_value, Return_code, Reason_code) __uss_generic (1068, 9, \
                         LockFcnCode, LockReqType, LockType, LockAddr, LockAttrAddr, LockTokenAddr, Return_value, Return_code, Reason_code)
#define __uss_shmget(Key, Shared_Memory_Size, Shared_Memory_Flags, Return_value, Return_code, Reason_code) __uss_generic (732, 6, Key, Shared_Memory_Size, Shared_Memory_Flags, Return_value, Return_code, Reason_code)
#define __uss_shutdown(Socket_descriptor, How, Return_value, Return_code, Reason_code) __uss_generic (572, 5, Socket_descriptor, How, Return_value, Return_code, Reason_code)
#define __uss_sigaction(Signal, New_sa_handler_address, New_sa_mask, New_sa_flags, Old_sa_handler_address, Old_sa_mask, Old_sa_flags, User_data, Return_value, Return_code, Reason_code) __uss_generic (324, 11, \
                        Signal, New_sa_handler_address, New_sa_mask, New_sa_flags, Old_sa_handler_address, Old_sa_mask, Old_sa_flags, User_data, Return_value, Return_code, Reason_code)
#define __uss___sigactionset(New_count, New_structure, Old_count, Old_structure, SsetOption_flags, Return_value, Return_code, Reason_code) __uss_generic (1084, 8, \
                             New_count, New_structure, Old_count, Old_structure, SsetOption_flags, Return_value, Return_code, Reason_code)
#define __uss_sigpending(Signal_pending_mask, Return_value, Return_code, Reason_code) __uss_generic (340, 4, Signal_pending_mask, Return_value, Return_code, Reason_code)
#define __uss_sigprocmask(How, New_signal_mask, Old_signal_mask, Return_value, Return_code, Reason_code) __uss_generic (352, 6, How, New_signal_mask, Old_signal_mask, Return_value, Return_code, Reason_code)
#define __uss_sigqueue(Process_ID, Signal, Signal_Value, Signal_Options, Return_value, Return_code, Reason_code) __uss_generic (1104, 7, Process_ID, Signal, Signal_Value, Signal_Options, Return_value, Return_code, Reason_code)
#define __uss_sigsuspend(Signal_mask, Return_value, Return_code, Reason_code) __uss_generic (360, 4, Signal_mask, Return_value, Return_code, Reason_code)
#define __uss_sigtimedwait(Signal_mask, Siginfo_ptr, Siginfo_len, Seconds, Nanoseconds, Return_value, Return_code, Reason_code) __uss_generic (1096, 8, \
                           Signal_mask, Siginfo_ptr, Siginfo_len, Seconds, Nanoseconds, Return_value, Return_code, Reason_code)
#define __uss_sigwait(Signal_mask, Return_value, Return_code, Reason_code) __uss_generic (468, 4, Signal_mask, Return_value, Return_code, Reason_code)
#define __uss_sleep(Seconds, Return_value) __uss_generic (344, 2, Seconds, Return_value)
#define __uss___smf_record(Smf_record_type, Smf_record_subtype, Smf_record_length, Smf_record_address, Return_value, Return_code, Reason_code) __uss_generic (1036, 7, \
                           Smf_record_type, Smf_record_subtype, Smf_record_length, Smf_record_address, Return_value, Return_code, Reason_code)
#define __uss_socket(Domain, Type, Protocol, Dimension, Socket_vector, Return_value, Return_code, Reason_code) __uss_generic (576, 8, Domain, Type, Protocol, Dimension, Socket_vector, Return_value, Return_code, Reason_code)
#define __uss_socket_pair(Domain, Type, Protocol, Dimension, Socket_vector, Return_value, Return_code, Reason_code) __uss_generic (576, 8, Domain, Type, Protocol, Dimension, Socket_vector, Return_value, Return_code, Reason_code)
#define __uss_spawn(Pathname_length, Pathname, Argument_count, Argument_length_list, Argument_list, Environment_count, Environment_data_length, Environment_data_list, Filedesc_count, Filedesc_list, Inherit_area_len, Inherit_area, \
                    Return_value, Return_code, Reason_code) __uss_generic (760, 15, Pathname_length, Pathname, Argument_count, Argument_length_list, Argument_list, Environment_count, Environment_data_length, Environment_data_list, \
                    Filedesc_count, Filedesc_list, Inherit_area_len, Inherit_area, Return_value, Return_code, Reason_code)
#define __uss_srx_np(Socket_descriptor, Direction, Msghdrx_length, Msghdrx, Return_value, Return_code, Reason_code) __uss_generic (1080, 7, Socket_descriptor, Direction, Msghdrx_length, Msghdrx, Return_value, Return_code, Reason_code)
#define __uss_stat(Pathname_length, Pathname, Status_area_length, Status_area, Return_value, Return_code, Reason_code) __uss_generic (192, 7, Pathname_length, Pathname, Status_area_length, Status_area, Return_value, Return_code, Reason_code)
#define __uss_StatVfs(Pathname_length, Pathname, Status_area_length, Status_area, Return_value, Return_code, Reason_code) __uss_generic (844, 7, Pathname_length, Pathname, Status_area_length, Status_area, Return_value, Return_code, Reason_code)
#define __uss_sw_signaldelv(signal_ind) __uss_generic (1124, 1, signal_ind)
#define __uss_symlink(Pathname_length, Pathname, Link_name_length, Link_name, Return_value, Return_code, Reason_code) __uss_generic (196, 7, Pathname_length, Pathname, Link_name_length, Link_name, Return_value, Return_code, Reason_code)
#define __uss_sync(Return_value, Return_code, Reason_code) __uss_generic (868, 3, Return_value, Return_code, Reason_code)
#define __uss_sysconf(Sysconf_name, Return_value, Return_code, Reason_code) __uss_generic (368, 4, Sysconf_name, Return_value, Return_code, Reason_code)
#define __uss_takesocket(Clientid, Socket_Id, Return_value, Return_code, Reason_code) __uss_generic (1032, 5, Clientid, Socket_Id, Return_value, Return_code, Reason_code)
#define __uss_tcdrain(File_descriptor, Return_value, Return_code, Reason_code) __uss_generic (24, 4, File_descriptor, Return_value, Return_code, Reason_code)
#define __uss_tcflow(File_descriptor, Action, Return_value, Return_code, Reason_code) __uss_generic (28, 5, File_descriptor, Action, Return_value, Return_code, Reason_code)
#define __uss_tcflush(File_descriptor, Queue_selector, Return_value, Return_code, Reason_code) __uss_generic (20, 5, File_descriptor, Queue_selector, Return_value, Return_code, Reason_code)
#define __uss_tcgetattr(File_descriptor, Termios_structure, Return_value, Return_code, Reason_code) __uss_generic (32, 5, File_descriptor, Termios_structure, Return_value, Return_code, Reason_code)
#define __uss_tcgetcp(File_descriptor, Termcp_length, Termcp_structure, Return_value, Return_code, Reason_code) __uss_generic (900, 6, File_descriptor, Termcp_length, Termcp_structure, Return_value, Return_code, Reason_code)
#define __uss_tcgetpgrp(File_descriptor, Return_value, Return_code, Reason_code) __uss_generic (36, 4, File_descriptor, Return_value, Return_code, Reason_code)
#define __uss_tcgetsid(File_descriptor, Return_value, Return_code, Reason_code) __uss_generic (912, 4, File_descriptor, Return_value, Return_code, Reason_code)
#define __uss_tcsendbreak(File_descriptor, Duration, Return_value, Return_code, Reason_code) __uss_generic (44, 5, File_descriptor, Duration, Return_value, Return_code, Reason_code)
#define __uss_tcsetattr(File_descriptor, Actions, Termios_structure, Return_value, Return_code, Reason_code) __uss_generic (40, 6, File_descriptor, Actions, Termios_structure, Return_value, Return_code, Reason_code)
#define __uss_tcsetcp(File_descriptor, Termcp_length, Termcp_structure, Return_value, Return_code, Reason_code) __uss_generic (904, 6, File_descriptor, Termcp_length, Termcp_structure, Return_value, Return_code, Reason_code)
#define __uss_tcsetpgrp(File_descriptor, Process_group_id, Return_value, Return_code, Reason_code) __uss_generic (48, 5, File_descriptor, Process_group_id, Return_value, Return_code, Reason_code)
#define __uss_tcsettables(File_descriptor, Termcp_length, Termcp_structure, Srctable, Trgtable, Return_value, Return_code, Reason_code) __uss_generic (908, 8, \
                          File_descriptor, Termcp_length, Termcp_structure, Srctable, Trgtable, Return_value, Return_code, Reason_code)
#define __uss_times(Time_data, Return_value, Return_code, Reason_code) __uss_generic (372, 4, Time_data, Return_value, Return_code, Reason_code)
#define __uss_truncate(Pathname_length, Pathname, File_length, Return_value, Return_code, Reason_code) __uss_generic (828, 6, Pathname_length, Pathname, File_length, Return_value, Return_code, Reason_code)
#define __uss_ttyname(File_descriptor, Terminal_name_length, Terminal_name) __uss_generic (16, 3, File_descriptor, Terminal_name_length, Terminal_name)
#define __uss_ttyname2(File_descriptor, Terminal_name_length, Terminal_name, Return_value, Return_code, Reason_code) __uss_generic (924, 6, File_descriptor, Terminal_name_length, Terminal_name, Return_value, Return_code, Reason_code)
#define __uss_umask(File_mode_creation_mask, Return_value) __uss_generic (204, 2, File_mode_creation_mask, Return_value)
#define __uss_umount(File_system_name, Flags, Return_value, Return_code, Reason_code) __uss_generic (208, 5, File_system_name, Flags, Return_value, Return_code, Reason_code)
#define __uss_uname(Data_area_length, Data_area_address, Return_value, Return_code, Reason_code) __uss_generic (376, 5, Data_area_length, Data_area_address, Return_value, Return_code, Reason_code)
#define __uss_unlink(Name_length, Name, Return_value, Return_code, Reason_code) __uss_generic (212, 5, Name_length, Name, Return_value, Return_code, Reason_code)
#define __uss_unlockpt(File_descriptor, Return_value, Return_code, Reason_code) __uss_generic (920, 4, File_descriptor, Return_value, Return_code, Reason_code)
#define __uss_unquiesce(File_system_name, Unquiesce_parms, Return_value, Return_code, Reason_code) __uss_generic (392, 5, File_system_name, Unquiesce_parms, Return_value, Return_code, Reason_code)
#define __uss_utime(Pathname_length, Pathname, Newtimes, Return_value, Return_code, Reason_code) __uss_generic (216, 6, Pathname_length, Pathname, Newtimes, Return_value, Return_code, Reason_code)
#define __uss_w_getipc(Ipc_Token_Member_ID, Buffer_Address, Buffer_Length, Command, Return_value, Return_code, Reason_code) __uss_generic (736, 7, Ipc_Token_Member_ID, Buffer_Address, Buffer_Length, Command, Return_value, Return_code, Reason_code)
#define __uss_w_getmntent(Buffer_length, Buffer, Return_value, Return_code, Reason_code) __uss_generic (76, 5, Buffer_length, Buffer, Return_value, Return_code, Reason_code)
#define __uss_w_getpsent(Process_token, Buffer_length, Buffer_address, Return_value, Return_code, Reason_code) __uss_generic (428, 6, Process_token, Buffer_length, Buffer_address, Return_value, Return_code, Reason_code)
#define __uss_w_ioctl(File_descriptor, Command, Argument_length, Argument, Return_value, Return_code, Reason_code) __uss_generic (120, 7, File_descriptor, Command, Argument_length, Argument, Return_value, Return_code, Reason_code)
#define __uss___wlm(FunctionCode, ParmListPtr, Return_value, Return_code, Reason_code) __uss_generic (1048, 5, FunctionCode, ParmListPtr, Return_value, Return_code, Reason_code)
#define __uss_w_pioctl(Pathname_length, Pathname, Command, Argument_length, Argument, Return_value, Return_code, Reason_code) __uss_generic (984, 8, \
                       Pathname_length, Pathname, Command, Argument_length, Argument, Return_value, Return_code, Reason_code)
#define __uss_w_statfs(File_system_name, Status_area_length, Status_area, Return_value, Return_code, Reason_code) __uss_generic (80, 6, File_system_name, Status_area_length, Status_area, Return_value, Return_code, Reason_code)
#define __uss_w_statvfs(File_system_name, Status_area_length, Status_area, Return_value, Return_code, Reason_code) __uss_generic (80, 6, File_system_name, Status_area_length, Status_area, Return_value, Return_code, Reason_code)
#define __uss_wait(Process_ID, Options, Status_field_address, Return_value, Return_code, Reason_code) __uss_generic (380, 6, Process_ID, Options, Status_field_address, Return_value, Return_code, Reason_code)
#define __uss_waitid(Function_code, Idtype, Id, Stat_loc_ptr, Options, Info_area_ptr, Return_value, Return_code, Reason_code) __uss_generic (840, 9, \
                     Function_code, Idtype, Id, Stat_loc_ptr, Options, Info_area_ptr, Return_value, Return_code, Reason_code)
#define __uss_write(File_descriptor, Buffer_address, Buffer_ALET, Write_count, Return_value, Return_code, Reason_code) __uss_generic (220, 7, File_descriptor, Buffer_address, Buffer_ALET, Write_count, Return_value, Return_code, Reason_code)
#define __uss_writev(File_descriptor, Iov_count, Iov_struct, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code) __uss_generic (580, 8, \
                     File_descriptor, Iov_count, Iov_struct, Iov_alet, Iov_buffer_alet, Return_value, Return_code, Reason_code)

/* The following had multiple names: */
#define __uss_getugrps(User_name_length, User_name, Group_ID_list_size, Group_ID_list_pointer_address, Number_of_group_IDs, Return_code, Reason_code) __uss_generic (292, 7, \
                       User_name_length, User_name, Group_ID_list_size, Group_ID_list_pointer_address, Number_of_group_IDs, Return_code, Reason_code)
#define __uss_getgroupsbyname(User_name_length, User_name, Group_ID_list_size, Group_ID_list_pointer_address, Number_of_group_IDs, Return_code, Reason_code) __uss_generic (292, 7, \
                              User_name_length, User_name, Group_ID_list_size, Group_ID_list_pointer_address, Number_of_group_IDs, Return_code, Reason_code)

#define __uss_pthread_xandg(Status_field, Options_field, Signal_setup_userdata, Return_value, Return_code, Reason_code) __uss_generic (436, 6, Status_field, Options_field, Signal_setup_userdata, Return_value, Return_code, Reason_code)
#define __uss_pthread_pthread_exit_and_get(Status_field, Options_field, Signal_setup_userdata, Return_value, Return_code, Reason_code) __uss_generic (436, 6, Status_field, Options_field, Signal_setup_userdata, Return_value, Return_code, Reason_code)

#define __uss_tsk_lvl_sec_np(Function_code, Identity_Type, Identity_Length, Identity, Password_Length, Password, Option_Flags, Return_value, Return_code, Reason_code) __uss_generic (964, 10, \
                             Function_code, Identity_Type, Identity_Length, Identity, Password_Length, Password, Option_Flags, Return_value, Return_code, Reason_code)
#define __uss_pthread_security_np(Function_code, Identity_Type, Identity_Length, Identity, Password_Length, Password, Option_Flags, Return_value, Return_code, Reason_code) __uss_generic (964, 10, \
                                  Function_code, Identity_Type, Identity_Length, Identity, Password_Length, Password, Option_Flags, Return_value, Return_code, Reason_code)

#define __uss_sigputback(Return_value, Return_code, Reason_code) __uss_generic (416, 3, Return_value, Return_code, Reason_code)
#define __uss_queue_interrupt(Return_value, Return_code, Reason_code) __uss_generic (416, 3, Return_value, Return_code, Reason_code)

#define __uss_set_limits(Action, MaxThreadTasks, MaxThreads, Return_value, Return_code, Reason_code) __uss_generic (684, 6, Action, MaxThreadTasks, MaxThreads, Return_value, Return_code, Reason_code)
#define __uss_set_thread_limits(Action, MaxThreadTasks, MaxThreads, Return_value, Return_code, Reason_code) __uss_generic (684, 6, Action, MaxThreadTasks, MaxThreads, Return_value, Return_code, Reason_code)

/* PC-only: ??? */
#define __uss_send_file(Sfpl_length, Sfpl, Return_value, Return_code, Reason_code) __uss_generic (1064, 5, Sfpl_length, Sfpl, Return_value, Return_code, Reason_code)

/* Generic wrapper for USS: */
extern void __uss_generic (int function, int parms, ...);

/* Unknown */

// IPCSDumpOpenClose    - BPXGMCDE - Dcor_Open, LevelIndicator, DumpDataSetName, LogDataSetName, ExecDataSetName, ClistDataSetName, DDIRStr, ErrorMsgPtr, Return_code, Return_Value1, Return_Value2, Return_Value3
// IPCSDumpOpenClose    - BPXGMCDE - DCOR_Close, OpenToken
// IPCSDumpAccess       - BPXGMPTR - Dcor_Request, OpenToken, Parm1Address, Parm2Address, Parm3Address, Return_value, Return_code, Reason_code

// <<<parms are not standard:>>> Semaphore_ID, Semaphore_Number Command, SValue | Argument_address ( Buffer | Array), Return_value, Return_code, Reason_code)
// #define __uss_semctl() __uss_generic (708, , )

/* Not defined - probably the same as the non v types though:
#define __uss_v_access() __uss_generic (944, 1, )
#define __uss_v_create() __uss_generic (620, 1, )
#define __uss_v_export() __uss_generic (876, 1, )
#define __uss_v_fstatfs() __uss_generic (656, 1, )
#define __uss_v_get() __uss_generic (596, 1, )
#define __uss_v_getattr() __uss_generic (632, 1, )
#define __uss_v_link() __uss_generic (640, 1, )
#define __uss_v_lockctl() __uss_generic (660, 1, )
#define __uss_v_lookup() __uss_generic (604, 1, )
#define __uss_v_mkdir() __uss_generic (624, 1, )
#define __uss_v_pathconf() __uss_generic (1040, 1, )
#define __uss_v_rdwr() __uss_generic (608, 1, )
#define __uss_v_readdir() __uss_generic (612, 1, )
#define __uss_v_readlink() __uss_generic (616, 1, )
#define __uss_v_reg() __uss_generic (584, 1, )
#define __uss_v_rel() __uss_generic (600, 1, )
#define __uss_v_rel() __uss_generic (600, 1, )
#define __uss_v_remove() __uss_generic (648, 1, )
#define __uss_v_rename() __uss_generic (652, 1, )
#define __uss_v_rmdir() __uss_generic (644, 1, )
#define __uss_v_rpn() __uss_generic (588, 1, )
#define __uss_v_settatr() __uss_generic (636, 1, )
#define __uss_v_symlink() __uss_generic (628, 1, )
*/

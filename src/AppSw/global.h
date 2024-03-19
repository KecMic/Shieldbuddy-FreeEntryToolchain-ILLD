#ifndef SRC_APPSW_GLOBAL_H_
#define SRC_APPSW_GLOBAL_H_


/**
 *	arr has to be declared as 'T arr[SIZE];' or defined as 'T arr[] = { ... };'.
 *	Does not work with arr declared as 'T arr[];' or with pointers 'T* arr'.
 */
#define ARRAY_NUM_ELEMS(arr)	(sizeof(arr)/sizeof(arr[0]))


#endif /* SRC_APPSW_GLOBAL_H_ */

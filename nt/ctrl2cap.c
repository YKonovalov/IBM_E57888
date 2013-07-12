//======================================================================
//
// ctrl2cap.c
//
// Copyright (C) 1996 Mark Russinovich
//
// Hook onto the keyboard I/O path and massage the input stream
// converting caps-locks into controls
//
//======================================================================
#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "ctrl2cap.h"
#include "ntddkbd.h"

// print macro that only turns on when checked builds are on
#if DBG
#define DbgPrint(arg) DbgPrint arg
#else
#define DbgPrint(arg)
#endif

NTSYSAPI
VOID
NTAPI
HalDisplayString( PCHAR String );


//----------------------------------------------------------------------
//                         GLOBALS
//---------------------------------------------------------------------- 
PDEVICE_OBJECT       kbdDevice;
PDEVICE_OBJECT       HookDeviceObject;

//----------------------------------------------------------------------
//                         FORWARD DEFINES
//---------------------------------------------------------------------- 
NTSTATUS Ctrl2capDispatchRead( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp );
NTSTATUS Ctrl2capDispatchGeneral(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp );
NTSTATUS Ctrl2capInit( IN PDRIVER_OBJECT DriverObject );


//----------------------------------------------------------------------
//
// DriverEntry
//
// Installable driver initialization. Here we just set ourselves up.
//
//----------------------------------------------------------------------
NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath )
{
    PDEVICE_OBJECT         deviceObject        = NULL;
    NTSTATUS               ntStatus;
    WCHAR                  deviceNameBuffer[]  = L"\\Device\\Ctrl2cap";
    UNICODE_STRING         deviceNameUnicodeString;
    WCHAR                  deviceLinkBuffer[]  = L"\\DosDevices\\Ctrl2cap";
    UNICODE_STRING         deviceLinkUnicodeString;

    DbgPrint (("Ctrl2cap.SYS: entering DriverEntry\n"));

    //
    // Create our device.
    //
	    
    RtlInitUnicodeString (&deviceNameUnicodeString,
                          deviceNameBuffer );
    
    ntStatus = IoCreateDevice (DriverObject,
                               0,
                               &deviceNameUnicodeString,
                               FILE_DEVICE_CTRL2CAP,
                               0,
                               TRUE,
                               &deviceObject );

    if (NT_SUCCESS(ntStatus)) {

	 //
	 // Create a symbolic link so that ctr2lcap can be dynamically loaded.
         //

	 RtlInitUnicodeString (&deviceLinkUnicodeString,
			       deviceLinkBuffer );
	 ntStatus = IoCreateSymbolicLink (&deviceLinkUnicodeString,
					  &deviceNameUnicodeString );
	 if (!NT_SUCCESS(ntStatus))
	      DbgPrint (("Ctrl2cap.SYS: IoCreateSymbolicLink failed\n"));

	 //
	 // Create dispatch points for all the IRPs the keyboard class device
         // handles.
         //

 	 DriverObject->MajorFunction[IRP_MJ_READ]	    
			   = Ctrl2capDispatchRead;
	 DriverObject->MajorFunction[IRP_MJ_CREATE]         =
	 DriverObject->MajorFunction[IRP_MJ_CLOSE]          =
	 DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS]  =
	 DriverObject->MajorFunction[IRP_MJ_CLEANUP]        =
	 DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] 
	                   = Ctrl2capDispatchGeneral;
    }

    if (!NT_SUCCESS(ntStatus)) {

	 //
	 // Something went wrong, so clean up (free resources etc)
	 //

         if (deviceObject )
	      IoDeleteDevice (deviceObject);
    }

    return Ctrl2capInit( DriverObject );
}


//----------------------------------------------------------------------
//
// Ctrl2capInit
//
// Hook onto the keyboard's path. Why does this routine return
// status success even if there's a problem? I've found that if it 
// doesn't, the keyboard won't respond!
//
//----------------------------------------------------------------------
NTSTATUS Ctrl2capInit( IN PDRIVER_OBJECT DriverObject )
{
    CCHAR		 ntNameBuffer[64];
    STRING		 ntNameString;
    UNICODE_STRING       ntUnicodeString;
    NTSTATUS             status;

    //
    // Only hook onto the first keyboard's chain.
    //

    sprintf( ntNameBuffer, "\\Device\\KeyboardClass0" );
    RtlInitAnsiString( &ntNameString, ntNameBuffer );
    RtlAnsiStringToUnicodeString( &ntUnicodeString, &ntNameString, TRUE );

    //
    // Create device object for the keyboard.
    //

    status = IoCreateDevice( DriverObject,
			     0,
			     NULL,
			     FILE_DEVICE_KEYBOARD,
			     0,
			     FALSE,
			     &HookDeviceObject );

    if( !NT_SUCCESS(status) ) {

	 DbgPrint(("Ctrl2cap: Keyboard hook failed to create device!\n"));

	 RtlFreeUnicodeString( &ntUnicodeString );

	 return STATUS_SUCCESS;
    }
   
    //
    // Keyboard uses buffered I/O so we must as well.
    //

    HookDeviceObject->Flags |= DO_BUFFERED_IO;

    //
    // Attach to the keyboard chain.
    //

    status = IoAttachDevice( HookDeviceObject, &ntUnicodeString, &kbdDevice );

    if( !NT_SUCCESS(status) ) {

	 DbgPrint(("Ctrl2cap: Connect with keyboard failed!\n"));

	 IoDeleteDevice( HookDeviceObject );

	 RtlFreeUnicodeString( &ntUnicodeString );
	
	 return STATUS_SUCCESS;
    }

    //
    // Done! Just free our string and be on our way...
    //

    RtlFreeUnicodeString( &ntUnicodeString );

    DbgPrint(("Ctrl2cap: Successfully connected to keyboard device\n"));

    //
    // This line simply demonstrates how a driver can print
    // stuff to the bluescreen during system initialization.
    //
  
    HalDisplayString( "Yurix IBM_E57888 Keyboard Support Initialized\n" );

    return STATUS_SUCCESS;
}

//----------------------------------------------------------------------
// 
// Ctrl2capReadComplete
//
// Gets control after a read operation has completed.
//
//----------------------------------------------------------------------
NTSTATUS Ctrl2capReadComplete( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp,
				IN PVOID Context )
{
    PIO_STACK_LOCATION        IrpSp;
    PKEYBOARD_INPUT_DATA      KeyData;
    int                       numKeys, i;

    //
    // Request completed - look at the result.
    //

    IrpSp = IoGetCurrentIrpStackLocation( Irp );
    if( NT_SUCCESS( Irp->IoStatus.Status ) ) {

        //
	// Do caps-lock down and caps-lock up. Note that
        // just frobbing the MakeCode handles both the up-key
        // and down-key cases since the up/down information is specified
        // seperately in the Flags field of the keyboard input data 
        // (0 means key-down, 1 means key-up).
        //

  	KeyData = Irp->AssociatedIrp.SystemBuffer;
        numKeys = Irp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);

        for( i = 0; i < numKeys; i++ ) {

            DbgPrint(("ScanCode: %x ", KeyData[i].MakeCode ));
            DbgPrint(("%s\n", KeyData[i].Flags ? "Up" : "Down" ));

	if ((KeyData[i].MakeCode < 128) && (IBM_keys[KeyData[i].MakeCode]!=0)) {

	        KeyData[i].MakeCode = IBM_keys[KeyData[i].MakeCode];
            } 
        }
    }

    if( Irp->PendingReturned ) {

        IoMarkIrpPending( Irp );

    }

    return Irp->IoStatus.Status;
}


//----------------------------------------------------------------------
//
// Ctrl2capDispatchRead
//
// Sets up to look at the read request completion so that we can
// massage the input queue on IO completion.
//
//----------------------------------------------------------------------
NTSTATUS Ctrl2capDispatchRead( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack    = IoGetNextIrpStackLocation(Irp);

    //
    // Push params down for keyboard class driver.
    //

    *nextIrpStack = *currentIrpStack;

    //  
    // Set the completion callback, so we can "frob" the keyboard data.
    //	    

    IoSetCompletionRoutine( Irp, Ctrl2capReadComplete, DeviceObject, TRUE, TRUE, TRUE );

    //
    // Return the results of the call to the keyboard class driver.
    //
	    
    return IoCallDriver( kbdDevice, Irp );
}


//----------------------------------------------------------------------
//
// Ctrl2capDispatchGeneral
//
// This handles several functions we are not interested in 
// along to the keyboard class driver. 
//
//----------------------------------------------------------------------
NTSTATUS Ctrl2capDispatchGeneral(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp )
{
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack    = IoGetNextIrpStackLocation(Irp);

    //
    // Default to success.
    //

    Irp->IoStatus.Status      = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    //
    // If this call was directed at the hook device, pass it to 
    // the keyboard class device, else handle it ourselves.
    // 

   if( DeviceObject == HookDeviceObject ) {

       *nextIrpStack = *currentIrpStack;

       return IoCallDriver( kbdDevice, Irp );

   } else {

       return STATUS_SUCCESS;

   }
}



#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 900
#define SEEK_SET 0
#define SEEK_CURR 1
#define SEEK_END 2
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yuri Fung");
MODULE_DESCRIPTION("PA3 Character Driver");
/* Define device_buffer and other global data structures you will need here */
int open_count = 0;
int close_count = 0;
char *device_buffer;
ssize_t pa3_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
    /* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
    /* length is the length of the userspace buffer*/
    /* offset will be set to current position of the opened file after read*/
    /* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
    //Reads outside buffer size
    if(*offset < 0){
        printk(KERN_ALERT "Error: No more Buffer Space \n");
        *offset = 0;
    }
    if(length > BUFFER_SIZE - *offset){
        printk(KERN_ALERT "Error: Can't read, no more buffer. It will read %zd bytes over limit\n",length);
        length = BUFFER_SIZE - *offset;//If length is greater than space remaining in buffer update value.
        // IF the offset + length here is bigger than BUFFER SIZE, we 
    //want to essential "Partial Read". WE set what we want to read to length = BUFFER_SIZE - offset.
    }
    if(*offset > BUFFER_SIZE){
        printk(KERN_ALERT "Error: No more Buffer Space. It will read %zd bytes over the limit\n", length);
        *offset = BUFFER_SIZE;
    }
    
    if (copy_to_user(buffer, device_buffer + *offset, length) != 0){
        printk(KERN_ALERT "Error: Failed to copy_to_user from kernel \n");
        return -1; 
    }
   
    
    *offset += length; //set new offset pos.
    printk(KERN_ALERT "Read %zd bytes from device \n", length);
    return length;
	// return 0;
}



ssize_t pa3_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	if(*offset < 0){
        printk(KERN_ALERT "Error: No more Buffer Space \n");
        *offset = 0;
    }
    if(length > BUFFER_SIZE - *offset){
        printk(KERN_ALERT "Error: Can't read, no more buffer. It will write %zd bytes over limit \n",length);
        length = BUFFER_SIZE - *offset;//If length is greater than space remaining in buffer update value.
        // IF the offset + length here is bigger than BUFFER SIZE, we 
    //want to essential "Partial Read". WE set what we want to read to length = BUFFER_SIZE - offset.
    }
    if(*offset > BUFFER_SIZE){
        printk(KERN_ALERT "Error: No more Buffer Space. It will write %zd bytes over the limit \n", length);
        *offset = BUFFER_SIZE;
    }
    
    if (copy_from_user((device_buffer + *offset),buffer, length)!=0){
        // printk(KERN_ALERT "Error: Failed to copy_to_user from kernel \n");
        return -1;  // Error copying data to user space
    }
    *offset += length; //set new offset pos.
    printk(KERN_ALERT "Wrote %zd bytes to device \n", length);
    return length;
    // return 0;
}


int pa3_char_driver_open (struct inode *pinode, struct file *pfile)
{
    /* print to the log file that the device is opened and also print the number of times this device has been opened until now*/        
	open_count++;
    printk(KERN_ALERT "Device opened %d times\n", open_count);
    return 0;
}

int pa3_char_driver_close (struct inode *pinode, struct file *pfile)
{
    /* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
    close_count++;
    printk(KERN_ALERT "Device closed %d times\n", close_count);
    return 0;
}

loff_t pa3_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
        /* Update open file position according to the values of offset and whence */
        
        
    loff_t currPos = 0;
    switch (whence) {
        case SEEK_SET:
            currPos = offset;
            // Set currPos as offset
            break;

        case SEEK_CUR:
            currPos = pfile->f_pos + offset;
            //set currPos when pos is incremented/decremented by offset bytes.
            break;

        case SEEK_END:
            currPos = BUFFER_SIZE + offset;
            //Set curPos = to end of file.
            break;

        default:
            return -EINVAL; // Invalid
    }

    if (currPos < 0) {
        // return -EINVAL; // Seek cannot move backwards.
        //if seek fails if before beginning or beyond the end of device, set curret pos to beginning/end of file.
        printk(KERN_ALERT "Position %lld not in range\n", currPos);
        currPos = 0; //set seek to beginning if it goes in negative offset.
    }else if(currPos > BUFFER_SIZE){
        printk(KERN_ALERT "Position %lld not in range\n", currPos);
        currPos = BUFFER_SIZE;//set to end if it goes past Buffer.
    }

    // Update the curPos
    pfile->f_pos = currPos;
    printk(KERN_ALERT "Device is now at position: %lld \n", currPos);
    return currPos;
	// return 0;

}

struct file_operations pa3_char_driver_file_operations = {
        /* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/

    .owner   = THIS_MODULE,
    .read = pa3_char_driver_read,
    .write = pa3_char_driver_write,
    .open = pa3_char_driver_open,
    .release = pa3_char_driver_close,
    .llseek = pa3_char_driver_seek,
};

static int pa3_char_driver_init(void)
{
        /* print to the log file that the init function is called.*/
        /* register the device */

    // printk(KERN_ALERT "Initialization started\n");
// If resudogister fails
    // if (register_chrdev(0, "simple_character_device", &pa3_char_driver_file_operations) < 0) {
    //     printk(KERN_ALERT "Error: Failed to register character device\n");
    //     return -EFAULT;
    // }
    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (register_chrdev(511, "simple_character_device", &pa3_char_driver_file_operations) < 0) {
        printk(KERN_ALERT "Error: Failed to register character device\n");
        return -EFAULT;
    }    
    printk(KERN_ALERT "Iniitialization Started: Character device registered\n");

    return 0;
}

static void pa3_char_driver_exit(void)
{
        /* print to the log file that the exit function is called.*/
        /* unregister  the device using the register_chrdev() function. */
    printk(KERN_ALERT "Exit function called\n");
    kfree(device_buffer);
    unregister_chrdev(511, "simple_character_device"); //Exit() 
    printk(KERN_ALERT "Character device unregistered\n");
	// return 0;
}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(pa3_char_driver_init);
module_exit(pa3_char_driver_exit);


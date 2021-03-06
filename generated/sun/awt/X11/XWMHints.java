// This file is an automatically generated file, please do not edit this file, modify the WrapperGenerator.java file instead !

package sun.awt.X11;

import sun.misc.*;

import java.util.logging.*;
public class XWMHints extends XWrapperBase { 
	private Unsafe unsafe = XlibWrapper.unsafe; 
	private final boolean should_free_memory;
	public static int getSize() { return 36; }
	public int getDataSize() { return getSize(); }

	long pData;

	public long getPData() { return pData; }


	XWMHints(long addr) {
		log.finest("Creating");
		pData=addr;
		should_free_memory = false;
	}


	XWMHints() {
		log.finest("Creating");
		pData = unsafe.allocateMemory(getSize());
		should_free_memory = true;
	}


	public void dispose() {
		log.finest("Disposing");
		if (should_free_memory) {
			log.finest("freeing memory");
			unsafe.freeMemory(pData); 
	}
		}
	public long get_flags() { log.finest("");return (Native.getLong(pData+0)); }
	public void set_flags(long v) { log.finest(""); Native.putLong(pData+0, v); }
	public int get_initial_state() { log.finest("");return (Native.getInt(pData+8)); }
	public void set_initial_state(int v) { log.finest(""); Native.putInt(pData+8, v); }
	public long get_icon_pixmap(int index) { log.finest(""); return Native.getLong(pData+12)+index*Native.getLongSize(); }
	public long get_icon_pixmap() { log.finest("");return Native.getLong(pData+12); }
	public void set_icon_pixmap(long v) { log.finest(""); Native.putLong(pData + 12, v); }
	public long get_icon_window() { log.finest("");return (Native.getLong(pData+16)); }
	public void set_icon_window(long v) { log.finest(""); Native.putLong(pData+16, v); }
	public int get_icon_x() { log.finest("");return (Native.getInt(pData+20)); }
	public void set_icon_x(int v) { log.finest(""); Native.putInt(pData+20, v); }
	public int get_icon_y() { log.finest("");return (Native.getInt(pData+24)); }
	public void set_icon_y(int v) { log.finest(""); Native.putInt(pData+24, v); }
	public long get_icon_mask() { log.finest("");return (Native.getLong(pData+28)); }
	public void set_icon_mask(long v) { log.finest(""); Native.putLong(pData+28, v); }
	public boolean get_input() { log.finest("");return (Native.getBool(pData+4)); }
	public void set_input(boolean v) { log.finest(""); Native.putBool(pData+4, v); }
	public long get_window_group() { log.finest("");return (Native.getLong(pData+32)); }
	public void set_window_group(long v) { log.finest(""); Native.putLong(pData+32, v); }


	String getName() {
		return "XWMHints"; 
	}


	String getFieldsAsString() {
		String ret="";

		ret += ""+"flags = " + get_flags() +", ";
		ret += ""+"initial_state = " + get_initial_state() +", ";
		ret += ""+"icon_pixmap = " + get_icon_pixmap() +", ";
		ret += ""+"icon_window = " + get_icon_window() +", ";
		ret += ""+"icon_x = " + get_icon_x() +", ";
		ret += ""+"icon_y = " + get_icon_y() +", ";
		ret += ""+"icon_mask = " + get_icon_mask() +", ";
		ret += ""+"input = " + get_input() +", ";
		ret += ""+"window_group = " + get_window_group() +", ";
		return ret;
	}


}




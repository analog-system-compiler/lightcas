/*
 * Copyright (C) 2006-2024 The LightCAS project                        
 *                                                                    
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or   
 * any later version.                                                  
 *                                                                    
 * This program is distributed in the hope that it will be useful,     
 * but WITHOUT ANY WARRANTY; without even the implied warranty of      
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       
 * GNU General Public License for more details.                        
 *                                                                    
 * You should have received a copy of the GNU General Public License   
 * along with this program; If not, see <https://www.gnu.org/licenses/>
 */


#include "Display.h"

FILE *CDisplay::m_DebugLogFile = nullptr;
char CDisplay::m_Buffer[2048];

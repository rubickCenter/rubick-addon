export interface Bitmap {
  width: number;
  height: number;
  image: any;
  byteWidth: number;
  bitsPerPixel: number;
  bytesPerPixel: number;
}

export interface Screen {
  capture(x?: number, y?: number, width?: number, height?: number): Bitmap;
  highlight(x: number, y: number, width: number, height: number, duration: number, opacity: number): void;
}

export interface Point {
  x: number;
  y: number
}

export interface Size {
  width: number;
  height: number;
}

export interface Rect {
  x: number;
  y: number;
  width: number;
  height: number;
}

export function setKeyboardDelay(ms: number): void;
export function keyTap(key: string, modifier?: string | string[]): void;
export function keyToggle(
  key: string,
  down: string,
  modifier?: string | string[]
): void;
export function typeString(string: string): void;
export function typeStringDelayed(string: string, cpm: number): void;
export function readFilePaths(): string[];

/**
 * Sets the focus to a specific window using its handle.
 *
 * @param {number} handle - The handle ID of the window to be focused.
 * @returns {void}
 */
export function focusWindow(handle: number): void

/**
* Resizes a window by its handle to the given width and height.
* The window is moved to the x & y coordinates if specified.
*
* @param {number} handle - The handle ID of the window to be resized.
* @param {Rect} rect - The new size of the window.
* @returns {void}
*/
export function resizeWindow(handle: number, rect: Rect): void 

export const screen: Screen;

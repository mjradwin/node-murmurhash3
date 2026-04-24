declare module 'murmurhash3' {
  /** Library version. */
  export const version: string;

  type Murmur32Callback = (err: Error | null, hash: number) => void;
  type Murmur32HexCallback = (err: Error | null, hash: string) => void;
  type Murmur128Callback = (err: Error | null, hash: number[]) => void;
  type Murmur128HexCallback = (err: Error | null, hash: string) => void;

  /** return 32bit integer value */
  export function murmur32(key: string, seed: number, callback: Murmur32Callback): void;
  export function murmur32(key: string, callback: Murmur32Callback): void;

  /** return 32bit hexadecimal string */
  export function murmur32Hex(key: string, seed: number, callback: Murmur32HexCallback): void;
  export function murmur32Hex(key: string, callback: Murmur32HexCallback): void;

  /** return array that have 4 elements of 32bit integer */
  export function murmur128(key: string, seed: number, callback: Murmur128Callback): void;
  export function murmur128(key: string, callback: Murmur128Callback): void;

  /** return 128bit hexadecimal string */
  export function murmur128Hex(key: string, seed: number, callback: Murmur128HexCallback): void;
  export function murmur128Hex(key: string, callback: Murmur128HexCallback): void;

  /** return 32bit integer value */
  export function murmur32Sync(key: string, seed?: number): number;
  /** return 32bit hexadecimal string */
  export function murmur32HexSync(key: string, seed?: number): string;
  /** return array that have 4 elements of 32bit integer */
  export function murmur128Sync(key: string, seed?: number): number[];
  /** return 128bit hexadecimal string */
  export function murmur128HexSync(key: string, seed?: number): string;
}

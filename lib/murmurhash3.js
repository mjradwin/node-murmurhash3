/*!
* murmurhash3
* Copyright(c) 2011 Hideaki Ohno <hide.o.j55{at}gmail.com>
* MIT Licensed
*/

import { createRequire } from 'module';
const require = createRequire(import.meta.url);

/**
 * Library version.
 */
export const version = '0.6.0';

let binding;
try {
  binding = require('../build/Release/murmurhash3');
} catch(e) {
  binding = require('../build/default/murmurhash3');
}

const E_CALLBACK_MISSING = "Callback function is missing";

//Async interface

export function murmur32(key, seed, cb) {
  if ( typeof seed == 'function') {
    cb = seed;
    seed = 0;
  }
  if (cb === undefined || typeof cb != 'function') {
    throw new Error(E_CALLBACK_MISSING);
  }
  const res = binding.murmur32(key, seed, false, function(err, res) {
    // Cast signed integer to unsigned integer
    cb(err, res >>> 0);
  });
  return res;
}

export function murmur32Hex(key, seed, cb) {
  if ( typeof seed == 'function') {
    cb = seed;
    seed = 0;
  }
  if (cb === undefined || typeof cb != 'function') {
    throw new Error(E_CALLBACK_MISSING);
  }
  return binding.murmur32(key, seed, true, cb);
}

export function murmur128(key, seed, cb) {
  if ( typeof seed == 'function') {
    cb = seed;
    seed = 0;
  }
  if (cb === undefined || typeof cb != 'function') {
    throw new Error(E_CALLBACK_MISSING);
  }
  binding.murmur128(key, seed, false, function(err, res) {
    for (let i = 0; i < res.length; i++) {
      // Cast signed integer to unsigned integer
      res[i] = res[i] >>> 0;
    }
    cb(err, res);
  });
}

export function murmur128Hex(key, seed, cb) {
  if ( typeof seed == 'function') {
    cb = seed;
    seed = 0;
  }
  if (cb === undefined || typeof cb != 'function') {
    throw new Error(E_CALLBACK_MISSING);
  }
  return binding.murmur128(key, seed, true, cb);
}


//Sync interface

export function murmur32Sync(key, seed) {
  // Cast signed integer to unsigned integer
  if (seed === undefined) {
    seed = 0;
  }
  const res = binding.murmur32Sync(key, seed, false) >>> 0;
  return res;
}

export function murmur32HexSync(key, seed) {
  if (seed === undefined) {
    seed = 0;
  }
  return binding.murmur32Sync(key, seed, true);
}

export function murmur128Sync(key, seed) {
  if (seed === undefined) {
    seed = 0;
  }
  const res = binding.murmur128Sync(key, seed, false);
  for (let i = 0; i < res.length; i++) {
    // Cast signed integer to unsigned integer
    res[i] = res[i] >>> 0;
  }
  return res;
}

export function murmur128HexSync(key, seed) {
  if (seed === undefined) {
    seed = 0;
  }
  return binding.murmur128Sync(key, seed, true);
}

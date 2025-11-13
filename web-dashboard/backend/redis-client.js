const net = require('net');

class RedisClient {
  constructor(host = '127.0.0.1', port = 6379) {
    this.host = host;
    this.port = port;
    this.socket = null;
    this.connected = false;
  }

  connect() {
    return new Promise((resolve, reject) => {
      this.socket = new net.Socket();
      
      this.socket.connect(this.port, this.host, () => {
        this.connected = true;
        console.log(`[Redis Client] Connected to ${this.host}:${this.port}`);
        resolve(true);
      });

      this.socket.on('error', (err) => {
        this.connected = false;
        console.error('[Redis Client] Error:', err.message);
        reject(err);
      });

      this.socket.on('close', () => {
        this.connected = false;
        console.log('[Redis Client] Connection closed');
      });
    });
  }

  disconnect() {
    if (this.socket) {
      this.socket.destroy();
      this.connected = false;
    }
  }

  buildRESPCommand(args) {
    let cmd = `*${args.length}\r\n`;
    for (const arg of args) {
      cmd += `$${arg.length}\r\n${arg}\r\n`;
    }
    return cmd;
  }

  parseRESPResponse(data) {
    const response = data.toString();
    
    // Simple string (+OK)
    if (response.startsWith('+')) {
      return response.substring(1, response.indexOf('\r\n'));
    }
    
    // Integer (:5)
    if (response.startsWith(':')) {
      return parseInt(response.substring(1, response.indexOf('\r\n')));
    }
    
    // Bulk string ($5\r\nhello\r\n)
    if (response.startsWith('$')) {
      const lines = response.split('\r\n');
      const len = parseInt(lines[0].substring(1));
      if (len === -1) return null;
      return lines[1];
    }
    
    // Array (*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n)
    if (response.startsWith('*')) {
      const lines = response.split('\r\n');
      const count = parseInt(lines[0].substring(1));
      if (count === -1) return null;
      
      const result = [];
      let i = 1;
      for (let j = 0; j < count; j++) {
        if (lines[i].startsWith('$')) {
          const len = parseInt(lines[i].substring(1));
          if (len !== -1) {
            result.push(lines[i + 1]);
            i += 2;
          } else {
            result.push(null);
            i += 1;
          }
        }
      }
      return result;
    }
    
    return response;
  }

  command(args) {
    return new Promise((resolve, reject) => {
      if (!this.connected) {
        return reject(new Error('Not connected to Redis'));
      }

      const cmd = this.buildRESPCommand(args);
      
      this.socket.once('data', (data) => {
        try {
          const result = this.parseRESPResponse(data);
          resolve(result);
        } catch (err) {
          reject(err);
        }
      });

      this.socket.write(cmd);
    });
  }

  async hget(key, field) {
    return await this.command(['HGET', key, field]);
  }

  async hgetall(key) {
    const result = await this.command(['HGETALL', key]);
    if (!result) return {};
    
    const obj = {};
    for (let i = 0; i < result.length; i += 2) {
      obj[result[i]] = result[i + 1];
    }
    return obj;
  }

  async hset(key, field, value) {
    return await this.command(['HSET', key, field, value]);
  }

  async lpush(key, value) {
    return await this.command(['LPUSH', key, value]);
  }

  async rpush(key, value) {
    return await this.command(['RPUSH', key, value]);
  }

  async llen(key) {
    return await this.command(['LLEN', key]);
  }

  async lrange(key, start, stop) {
    return await this.command(['LRANGE', key, start, stop]);
  }

  async keys(pattern) {
    return await this.command(['KEYS', pattern]);
  }

  async del(key) {
    return await this.command(['DEL', key]);
  }

  async ping() {
    try {
      const result = await this.command(['PING']);
      return result === 'PONG';
    } catch {
      return false;
    }
  }
}

module.exports = RedisClient;

//------------------------------------------------------------------------------
//       Copyright 2014-2019 Creepy Doll Games LLC. All rights reserved.
//
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
// NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include<generators.h>
#include<luacore.h>
#include<std.h>

extern"C"{
  #include<lua/lauxlib.h>
  #include<lua/lua.h>
}

using namespace EON;
using namespace gfc;

//================================================|=============================
//Lua:{                                           |
  //Extends:{                                     |

#ifdef __APPLE__
  #pragma mark - Lua -
#endif

    e_extends( Lua );

  //}:                                            |
  //Methods:{                                     |
    //Memory management:{                         |

#ifdef __APPLE__
  #pragma mark - Memory management -
#endif

      vp Lua::allocate( vp/* ud */, vp ptr, size_t/* osize */, size_t nsize ){
        if( nsize ){
          return e_realloc( ptr, u32( nsize ));
        }
        e_free( ptr );
        return 0;
      }

    //}:                                          |
    //Debugging:{                                 |

      #if 1
        extern "C" {
          void luaG_runerror( lua_State* /*L*/, ccp fmt,... ){
            #if !e_compiling( web )
              va_list argp;
              va_start( argp, fmt );
                char text[ 4096 ];
                #if e_compiling( microsoft )
                  vsprintf_s( text, e_dimof( text ), fmt, argp );
                #else
                  vsnprintf( text, e_dimof( text ), fmt, argp );
                #endif
                e_brk( "$(red)" + string( text ));
              va_end( argp );
            #endif
          }
        }
      #endif

    //}:                                          |
    //Standard:{                                  |
      //collectgarbage:{                          |

#ifdef __APPLE__
  #pragma mark - Standard library -
#endif

        int Lua::collectgarbage( lua_State* L ){
          static const char *const opts[]={
            "stop",
            "restart",
            "collect",
            "count",
            "step",
            "setpause",
            "setstepmul",
            "isrunning",
            NULL
          };
          static const int optsnum[]={
            LUA_GCSTOP,
            LUA_GCRESTART,
            LUA_GCCOLLECT,
            LUA_GCCOUNT,
            LUA_GCSTEP,
            LUA_GCSETPAUSE,
            LUA_GCSETSTEPMUL,
            LUA_GCISRUNNING
          };
          const int o = optsnum[ luaL_checkoption( L, 1, "collect", opts )];
          const int ex = int( luaL_optinteger( L, 2, 0 ));
          const int res = lua_gc( L, o, ex );
          switch( o ){
            case LUA_GCCOUNT:/**/{
              int b = lua_gc( L, LUA_GCCOUNTB, 0 );
              lua_pushnumber( L, (lua_Number)res + ((lua_Number)b/1024) );
              return 1;
            }
            case LUA_GCSTEP:
            case LUA_GCISRUNNING:/**/{
              lua_pushboolean( L, res );
              return 1;
            }
            default:/**/{
              lua_pushinteger( L, res );
              return 1;
            }
          }
        }

      //}:                                        |
      //setmetatable:{                            |

        int Lua::setmetatable( lua_State* L ){
          int t = lua_type( L, 2 );
          luaL_checktype( L, 1, LUA_TTABLE );
          luaL_argcheck( L, t == LUA_TNIL || t == LUA_TTABLE, 2, "nil or table expected" );
          if (luaL_getmetafield( L, 1, "__metatable") != LUA_TNIL )
            return luaL_error( L, "cannot change a protected metatable" );
          lua_settop( L, 2 );
          lua_setmetatable( L, 1 );
          return 1;
        }

      //}:                                        |
      //getmetatable:{                            |

        int Lua::getmetatable( lua_State* L ){
          luaL_checkany(L, 1);
          if( !lua_getmetatable( L, 1 )){
            lua_pushnil( L );
            return 1;
          }
          luaL_getmetafield( L, 1, "__metatable" );
          return 1;
        }

      //}:                                        |
      //loadstring:{                              |

        int Lua::loadstring( lua_State* L ){
          ccp spec = lua_tostring( L, lua_gettop( L ));
          if( spec ){
            sandbox( L, spec );
          }
          return 0;
        }

      //}:                                        |
      //tostring:{                                |

        int Lua::tostring( lua_State* L ){
          switch( lua_type( L, -1 )){
            case LUA_TSTRING:
              lua_pushstring( L, lua_tostring( L, -1 ));
              return 1;
            case LUA_TNIL:
              lua_pushstring( L, "nil" );
              return 1;
            case LUA_TBOOLEAN:
              lua_pushstring( L, e_xfs( "%s", lua_toboolean( L, -1 ) ? "true" : "false" ));
              return 1;
            case LUA_TLIGHTUSERDATA:
              lua_pushstring( L, "<lightuserdata>" );
              return 1;
            case LUA_TNUMBER:
              lua_pushstring( L, e_xfs( "%f", lua_tonumber( L, -1 )));
              return 1;
            case LUA_TTABLE:
              lua_pushstring( L, "<table>" );
              return 1;
            case LUA_TFUNCTION:
              lua_pushstring( L, "<function>" );
              return 1;
            case LUA_TUSERDATA:
              lua_pushstring( L, "<userdata>" );
              return 1;
            case LUA_TTHREAD:
              lua_pushstring( L, "<thread>" );
              return 1;
          }
          return 0;
        }

      //}:                                        |
      //require:{                                 |

        void Lua::require( ccp filename ){
          if( !e_fexists( filename )){
            e_errorf( 8272272, "file not found: %s", filename );
            return;
          }
          lua_pushstring( L, filename );
             require( L );
          lua_pop( L, 1 );
        }

        int Lua::require( lua_State* L ){
          ccp spec = lua_tostring( L, -1 );
          if( spec ){
            string filename;
            filename << spec;
            if( filename.ext().tolower().hash() != e_hashstr64_const( ".lua" )){
              filename << ".lua";
            }
            #if 0
              switch( luaL_loadfile( L, filename )){
                case LUA_ERRSYNTAX:
                  e_logf( "Syntax error in %s", filename.c_str() );
                  break;
                case LUA_ERRFILE:
                  e_logf( "Couldn't find file in %s", filename.c_str() );
                  break;
                case LUA_ERRMEM:
                  e_logf( "OOM error in %s", filename.c_str() );
                  break;
                case LUA_OK:
                  lua_getglobal( L, "__sandbox" );
                  lua_setupvalue( L, -2, 1 );
                  lua_call( L, 0, 0 );
                  break;
              }
            #else
              const auto& f = e_fload( filename );
              if( !f.empty() ){
                f.query(
                  [=]( ccp pFile ){
                    sandbox( L, pFile );
                  }
                );
              }
            #endif
          }
          return 0;
        }

      //}:                                        |
      //select:{                                  |

        int Lua::select( lua_State* L ){
          int n = lua_gettop( L );
          if( lua_type( L, 1 ) == LUA_TSTRING && *lua_tostring( L, 1 ) == '#' ){
            lua_pushinteger( L, n-1 );
            return 1;
          }
          lua_Integer i = luaL_checkinteger( L, 1 );
          if( i < 0 ){
            i = n + i;
          }else if (i > n){
            i = n;
          }
          luaL_argcheck( L, 1 <= i, 1, "index out of range" );
          return n - (int)i;
        }

      //}:                                        |
      //pairs:{                                   |

        namespace{
          int pairsmeta( lua_State* L, const char* method, int iszero, lua_CFunction iter ){
            if( luaL_getmetafield( L, 1, method ) == LUA_TNIL ){
              luaL_checktype( L, 1, LUA_TTABLE );
              lua_pushcfunction( L, iter );
              lua_pushvalue( L, 1 );
              if( iszero ){
                lua_pushinteger( L, 0 );
              }else{
                lua_pushnil( L );
              }
            }else{
              lua_pushvalue( L, 1 );
              lua_call( L, 1, 3 );
            }
            return 3;
          }
        }

        int Lua::pairs( lua_State* L ){
          return pairsmeta( L, "__pairs", 0, next );
        }

      //}:                                        |
      //error:{                                   |

        int Lua::error( lua_State* L ){
          e_hlt( lua_tostring( L, lua_gettop( L )));
          return 0;
        }

      //}:                                        |
      //print:{                                   |

        int Lua::print( lua_State* L ){
          if( !lua_istable( L, -1 )){
            #if e_compiling( android )
              e_log( lua_tostring( L, -1 ));
            #else
              e_msgf( "%s", lua_tostring( L, -1 ));
            #endif
          }else{
            //todo: dump table to log
          }
          return 0;
        }

      //}:                                        |
      //type:{                                    |

        int Lua::type( lua_State* L ){
          ccp name = luaL_typename( L, -1 );
          lua_pushstring( L, name );
          return 1;
        }

      //}:                                        |
      //next:{                                    |

        int Lua::next( lua_State* L ){
          luaL_checktype( L, 1, LUA_TTABLE );
          lua_settop( L, 2 );  /* create a 2nd argument if there isn't one */
          if( lua_next( L, 1 )){
            return 2;
          }
          lua_pushnil( L );
          return 1;
        }

      //}:                                        |
    //}:                                          |
    //Registry:{                                  |
      //set*:{                                    |

        void Lua::set( ccp key, const string& value ){
          lua_getglobal( L, "__sandbox" );
          lua_pushstring( L, value );
          lua_setfield( L, -2, key );
          lua_pop( L, 1 );
        }

        void Lua::set( ccp key, ccp value ){
          lua_getglobal( L, "__sandbox" );
          lua_pushstring( L, value );
          lua_setfield( L, -2, key );
          lua_pop( L, 1 );
        }

        void Lua::set( ccp key, s64 value ){
          lua_getglobal( L, "__sandbox" );
          lua_pushinteger( L, value );
          lua_setfield( L, -2, key );
          lua_pop( L, 1 );
        }

        void Lua::set( ccp key, f32 value ){
          lua_getglobal( L, "__sandbox" );
          lua_pushnumber( L, f32::cast( value ));
          lua_setfield( L, -2, key );
          lua_pop( L, 1 );
        }

      //}:                                        |
      //to*:{                                     |

        s64 Lua::toInteger( ccp key )const{
          lua_getglobal( L, "__sandbox" );
          if( lua_isnil( L, -1 )){
            e_unreachable( "too early to call toInteger" );
          }
          lua_getfield( L, -1, key );
          s64 value = lua_tointeger( L, -1 );
          lua_pop( L, 2 );
          return value;
        }

        string Lua::toString( ccp key )const{
          lua_getglobal( L, "__sandbox" );
          if( lua_isnil( L, -1 )){
            e_unreachable( "too early to call toInteger" );
          }
          lua_getfield( L, -1, key );
          string value = lua_tostring( L, -1 );
          lua_pop( L, 2 );
          return value;
        }

        ccp Lua::toCstring( ccp key )const{
          lua_getglobal( L, "__sandbox" );
          if( lua_isnil( L, -1 )){
            e_unreachable( "too early to call toInteger" );
          }
          lua_getfield( L, -1, key );
          ccp value = lua_tostring( L, -1 );
          lua_pop( L, 2 );
          return value;
        }

        f32 Lua::toFloat( ccp key )const{
          lua_getglobal( L, "__sandbox" );
          if( lua_isnil( L, -1 )){
            e_unreachable( "too early to call toInteger" );
          }
          lua_getfield( L, -1, key );
          f32 value = lua_tonumber( L, -1 );
          lua_pop( L, 2 );
          return value;
        }

      //}:                                        |
    //}:                                          |
    //Session:{                                   |
      //initialise:{                              |

#ifdef __APPLE__
  #pragma mark - Session -
#endif

        void Lua::initialise(){
          destroy();
          L = lua_newstate( allocate, 0 );
          luaL_Reg regSandbox[]={
            /* standard lua keywords */
            {"collectgarbage", collectgarbage},
            {"setmetatable", setmetatable},
            {"getmetatable", getmetatable},
            {"loadstring", loadstring},
            {"tostring", tostring},
            {"require", require},
            {"select", select},
            {"error", error},
            {"print", print},
            {"type", type},
            {"next", next},
            {"pairs", pairs},
            /* engine extensions */
            /* terminate */
            {0,0}
          };
          lua_createtable( L, e_dimof( regSandbox ), 0 );
          luaL_newlib( L, regSandbox );
          lua_pushvalue( L, -1 );
          lua_setglobal( L, "__sandbox" );
          //cannot store UUID because it hasn't been set yet.
          lua_pushlightuserdata( L, this );
          lua_setfield( L, -2, "this" );
          lua_pop( L, 1 );
          string startupSequence=/*
            class'name'{
               name=function(self)
               end,
            }*/
            "class=function(T)\n"
            "  if type(T)=='string'then\n"
            "    local mt={\n"
            "      __call=function(self,t,...)\n"
            "        local tt=type(t)\n"
            "        if tt=='table'then\n"
            "          for k,v in pairs(t)do\n"
            "            self[k]=v\n"
            "          end\n"
            "        elseif tt=='string'then\n"
            "          return new(self.__class,t,...)\n"
            "        end\n"
            "        return self\n"
            "      end,\n"
            "    }\n"
            "    _ENV[T]=setmetatable({},mt)\n"
            "    _ENV[T].__class=T\n"
            "    return _ENV[T]\n"
            "  end\n"
            "end\n"
            // obj=new(T,...)
            "new=function(T,...)\n"
            "  local copyMethods=function(obj,t)\n"
            "    for k,v in pairs(t)do\n"
            "      if type(v)=='function'then\n"
            "        obj[k]=v\n"
            "      end\n"
            "    end\n"
            "    return obj\n"
            "  end\n"
            "  local obj={}\n"
            "  if type(T)=='string'then\n"
            "    copyMethods(obj,_ENV[T])\n"
            "    if type(obj[T])=='function'then\n"
            "      obj[T](obj,...)\n"
            "      obj[T]=nil\n"
            "    end\n"
            "  elseif type(T)=='table'then\n"
            "    copyMethods(obj,T)\n"
            "    if type(obj[T.__class])=='function'then\n"
            "      obj[T.__class](obj,...)\n"
            "      obj[T.__class]=nil\n"
            "    end\n"
            "  end\n"
            "  return obj\n"
            "end\n"
            // Unit tests.
            #if e_compiling( debug )&& 0
              "print'Creating test class'\n"
              "class'test1'{\n"
              "  test1=function(self,name)\n"
              "    print('hello '..name)\n"
              "  end\n"
              "}\n"
              "class'test2'\n"
              "function test2:test2(name)\n"
              "  print('bye '..name)\n"
              "end\n"
              "class'test3'.test3=function(self,name)\n"
              "  print('silly '..name)\n"
              "end\n"
              "obj = new('test1','bob')\n"
              "obj = new(test2,'dad')\n"
              "obj = test3'billy'\n"
              "test1 = nil\n"
              "test2 = nil\n"
              "test3 = nil\n"
              "obj = nil\n"
            #endif
          ;
          sandbox( startupSequence );
          classify( "out", generators );
          classify( "gfc", standard );
        }

      //}:                                        |
      //destroy:{                                 |

        void Lua::destroy(){
          if( L ){
            lua_close( L );
            L = 0;
          }
        }

      //}:                                        |
      //uuid:{                                    |

        s64 Lua::uuid( lua_State* L ){
          lua_getglobal( L, "UUID" );
          s64 UUID = lua_tointeger( L, -1 );
          lua_pop( L, 1 );
          return UUID;
        }

      //}:                                        |
      //exit:{                                    |

        void Lua::exit(){
          int exit = findInSandbox( "exit" );
          if( exit ){
            call( exit );
          }
        }

      //}:                                        |
      //main:{                                    |

        void Lua::main(){
          int main = findInSandbox( "main" );
          if( main ){
            call( main );
          }
        }

      //}:                                        |
    //}:                                          |
    //API:{                                       |
      //getFieldAlways:{                          |

#ifdef __APPLE__
  #pragma mark - API -
#endif

        bool Lua::getFieldAlways( lua_State* L, ccp pField ){
          if( !lua_istable( L, -1 )){
            return false;
          }
          lua_getfield( L, -1, pField );
          if( lua_isnil( L, -1 )){
            lua_pop( L, 1 );
            lua_createtable( L, 0, 0 );
            lua_pushvalue( L, -1 );
            lua_setfield( L, -3, pField );
          }
          return true;
        }

      //}:                                        |
      //classify:{                                |

        void Lua::classify( ccp clsid, const luaL_Reg* methods ){

          //produce a new class, e.g., class'entity'
          int cls = findInSandbox( "class" );
          lua_rawgeti( L, LUA_REGISTRYINDEX, cls );
          lua_pushstring( L, clsid );
          lua_call( L, 1, 0 );
          unref( cls );

          //pass {methods} to return value set in _ENV[T] or
          //__sandbox.entity{methods} using __call.
          cls = findInSandbox( clsid );
          lua_rawgeti( L, LUA_REGISTRYINDEX, cls );
          lua_createtable( L, 0, 0 );
          luaL_setfuncs( L, methods, 0 );
          lua_call( L, 1, 0 );
          unref( cls );
        }

      //}:                                        |
      //sandbox:{                                 |

        bool Lua::sandbox( lua_State* L, ccp pScript ){

          //--------------------------------------------------------------------
          // Handle the "if/elif/endif" preprocessor statement.
          //--------------------------------------------------------------------

          static const auto& onConditional = [](
                string& script
              , ccp p
              , ccp s
              , ccp e ){
            ccp keyw = string::skip_anynonws( s );
            ccp labl = string::skip_anyws( keyw );
            if( labl ){
              ccp end = string::skip_anynonws( labl );
              if( end ){
                const auto& label = string( labl, end );
                if( !label.empty() ){
                  auto expr = string(
                      string::skip_ws( end )
                    , e );
                  const auto& token = string( s, keyw );
                  switch( token.hash() ){
                    case"endif"_64:/**/{
                      const auto& p0 = string( script.c_str(), p );
                      auto p1 = string( e );
                      script = p0 + p1;
                      return true;
                    }
                    case"elif"_64:/**/{
                      const auto& p0 = string( script.c_str(), p );
                      auto p1 = string( e );
                      script = p0 + p1;
                      return true;
                    }
                    case"else"_64:/**/{
                      const auto& p0 = string( script.c_str(), p );
                      auto p1 = string( e );
                      script = p0 + p1;
                      return true;
                    }
                    case"if"_64:/**/{
                      const auto& p0 = string( script.c_str(), p );
                      auto p1 = string( e );
                      script = p0 + p1;
                      return true;
                    }
                  }
                }
              }
            }
            return false;
          };

          //--------------------------------------------------------------------
          // Define an equate in the vein of C# not C/C++. Use the #macro
          // directive instead for proper macros; like old school MASM.
          //--------------------------------------------------------------------

          static const auto& onDefine = [](
                string& script
              , ccp p
              , ccp s
              , ccp e ){
            ccp keyw = string::skip_anynonws( s );
            ccp labl = string::skip_anyws( keyw );
            if( labl ){
              ccp end = string::skip_anynonws( labl );
              if( end ){
                const auto& label = string( labl, end );
                if( !label.empty() ){
                  auto value = string(
                      string::skip_ws( end )
                    , e );
                  const auto& token = string( s, keyw );
                  switch( token.hash() ){
                    case"define"_64:
                      value.del( "\"" );
                      value.del( " " );
                      const auto& p0 = string( script.c_str(), p );
                      auto p1 = string( e );
                      p1.replace(
                          "${" + label + "}"
                        , value );
                      script = p0 + p1;
                      return true;
                  }
                }
              }
            }
            return false;
          };

          //--------------------------------------------------------------------
          // Include other Lua scripts doesn't sandbox but is a textual replace
          // instead.
          //--------------------------------------------------------------------

          static const auto& onInclude = [](
                string& script
              , ccp p
              , ccp s
              , ccp e ){
            ccp keyw = string::skip_anynonws( s );
            ccp path = string::skip_anyws( keyw );
            if( path ){
              if( *path == '<' ){
                path = ccp( path )+1;
                cp end = cp( strchr( path, '>' ));
                if( !end ){
                  e_errorf( 1981222,
                    "syntax error in preprocessor: non-matching <> in "
                    "preprocessor: #include." );
                  return false;
                }
                *end = 0;
              }else if( *path == '"' ){
                path = ccp( path )+1;
                cp end = cp( strchr( path, '"' ));
                if( !end ){
                  e_errorf( 3783555,
                    "syntax error in preprocessor: non-matching \"\" in "
                    "preprocessor: #include." );
                  return false;
                }
                *end = 0;
              }else{
                return false;
              }
            }
            const auto& token = string( s, keyw );
            switch( token.hash() ){
              case"include"_64:/**/{
                const auto& st = e_fload( path );
                if( !st.empty() ){
                  const auto& ld = string( st.data() );
                  const auto& p0 = string( script.c_str(), p );
                  const auto& p1 = string( e );
                  script = p0 + ld + p1;
                }else{
                  e_errorf( 18276364, "couldn't load %s", ccp( path ));
                  const auto& p0 = string( script.c_str(), p );
                  const auto& p1 = string( e );
                  script = p0 + p1;
                }
                return true;
              }
            }
            e_errorf( 2892333,
              "syntax error in preprocessor: non-matching <> in "
              "preprocessor statement." );
            return false;
          };

          //--------------------------------------------------------------------
          // Process the incoming script.
          //--------------------------------------------------------------------

          string script( pScript );
          if( !script.empty() ){
            while( true ){
              ccp s = script.c_str();
              if( !strchr( s, '#' )){
                break;
              }
              ccp z = script.end();
              while( s && *s ){
                ccp tag = strchr( s, '#' );
                if( !tag ){
                  goto sk;
                }
                ccp e = strchr( tag, '\n' );
                if( !e ){
                  e = z;
                }
                if( onInclude( script
                    , tag
                    , string::skip_ws( tag+1 )
                    , e )){
                  break;
                }
                if( onDefine( script
                    , tag
                    , string::skip_ws( tag+1 )
                    , e )){
                  break;
                }
                if( onConditional( script
                    , tag
                    , string::skip_ws( tag+1 )
                    , e )){
                  break;
                }
              }
            }
          }

          //--------------------------------------------------------------------
          // Compile up the script we just processed by passing to Lua.
          //--------------------------------------------------------------------

sk:       if( !script.empty() ){
            const int err = luaL_loadstring( L, script );
            switch( err ){
              case LUA_ERRSYNTAX:
                e_logf( "LUA_ERRSYNTAX: %s", lua_tostring( L, -1 ));
                break;
              case LUA_ERRMEM:
                e_logf( "LUA_ERRMEM: %s", lua_tostring( L, -1 ));
                break;
              case LUA_OK:
                lua_getglobal( L, "__sandbox" );
                lua_setupvalue( L, -2, 1 );
                lua_call( L, 0, 0 );
                return true;
            }
            lua_pop( L, 1 );
          }
          return false;
        }

        bool Lua::sandbox( ccp s ){
          return sandbox( L, s );
        }

      //}:                                        |
      //unref:{                                   |

        void Lua::unref( int ref )const{
          if( ref ){
            luaL_unref( L, LUA_REGISTRYINDEX, ref );
          }
        }

      //}:                                        |
      //find:{                                    |

        int Lua::find( int ref, ccp name )const{
          int run = 0;
          lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
          if( lua_istable( L, -1 )){
            lua_pushnil( L );
            while( lua_next( L, -2 )){
              lua_pushvalue( L, -2 );
              if( lua_isstring( L, -1 )){
                ccp key=lua_tostring( L, -1 );
                if( !strcmp( key, name )){
                  lua_pushvalue( L, -2 );
                  run = luaL_ref( L, LUA_REGISTRYINDEX );
                  lua_pop( L, 2 );
                  break;
                }
              }
              lua_pop( L, 2 );
            }
          }
          lua_pop( L, 1 );
          return run;
        }

        int Lua::findInTable( int ref, ccp pName )const{
          int run = 0;
          lua_rawgeti( L, LUA_REGISTRYINDEX, ref );//+1
          if( lua_istable( L, -1 )){
            lua_getfield( L, -1, pName );//+1
            if( !lua_isnil( L, -1 )){
              run = luaL_ref( L, LUA_REGISTRYINDEX );//-1
            }else{
              lua_pop( L, 1 );//-1
            }
          }
          lua_pop( L, 1 );//-1
          return run;
        }

        int Lua::findInGlobal( ccp pGlobalTableName, ccp pName )const{
          lua_getglobal( L, pGlobalTableName );//+1
          if( lua_istable( L, -1 )){
            int table = luaL_ref( L, LUA_REGISTRYINDEX );//-1
            int value = findInTable( table, pName );
            unref( table );
            return value;
          }
          lua_pop( L, 1 );//-1
          return 0;
        }

        int Lua::findInSandbox( ccp pName )const{
          return findInGlobal( "__sandbox", pName );
        }

      //}:                                        |
      //call:{                                    |

        void Lua::call( const int ref, const int run, const f32& value ){
          if( ref && run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref ); //arg #1
            lua_pushnumber( L, f32::cast( value ));
            lua_call( L, 2, 0 );
          }
        }

        void Lua::call( const int ref, const int run, const s64 value ){
          if( ref && run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref ); //arg #1
            lua_pushinteger( L, value );
            lua_call( L, 2, 0 );
          }
        }

        void Lua::call( const int ref, const int run ){
          if( ref && run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_rawgeti( L, LUA_REGISTRYINDEX, ref ); //arg #0
            lua_call( L, 1, 0 );
          }
        }

        void Lua::call( const int run, const s64 value ){
          if( run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_pushinteger( L, value );
            lua_call( L, 1, 0 );
          }
        }

        void Lua::call( const int run, const f32& value ){
          if( run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_pushnumber( L, f32::cast( value ));
            lua_call( L, 1, 0 );
          }
        }

        void Lua::call( const int run ){
          if( run ){
            lua_rawgeti( L, LUA_REGISTRYINDEX, run );
            lua_call( L, 0, 0 );
          }
        }

      //}:                                        |
    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================|=============================

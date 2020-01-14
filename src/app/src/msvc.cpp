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
#include<ws.h>
#include<regex>

using namespace EON;
using namespace gfc;
using namespace fs;

//================================================|=============================
//Configs:{                                       |

  namespace{
    constexpr ccp anon_aConfigs[2]{ "Debug", "Release" };
  }

//}:                                              |
//Private:{                                       |
  //ignoreFile:{                                  |

    namespace{
      bool anon_ignoreFile( const string& regex, const string& s ){
        if( regex.empty() ){
          return false;
        }
        const std::regex r( regex.c_str() );
        const std::string var( s );
        auto it = var.cbegin();
        std::smatch sm;
        if( std::regex_search( it, var.cend(), sm, r )){
          return true;
        }
        return false;
      }
    }

  //}:                                            |
//}:                                              |
//Methods:{                                       |
  //[project]:{                                   |
    //extFromSource<>:{                           |

      ccp Workspace::MSVC::extFromEnum( const Type e )const{
        switch( e ){
          case decltype( e )::kCpp:
            return ".cpp";
          case decltype( e )::kC:
            return ".c";
          default:
            return "";
        }
      }

    //}:                                          |
    //sortingHat:{                                |

      void Workspace::MSVC::sortingHat( const string& in_path ){

        //----------------------------------------------------------------------
        // Solutions need Windows GUIDs. If we're running on Windows then you
        // are guaranteed to have a GUID that's universally unique. If running
        // on the Mac or Linux, you're only guaranteed to have file uniqueness.
        //----------------------------------------------------------------------

        File path = in_path;
        path.setBuildID( string::guid() );
        path.setRefID(   string::guid() );
        const auto& ext = path.ext().tolower();
        switch( ext.hash() ){

          //--------------------------------------------------------------------
          // Platform specific file types.
          //--------------------------------------------------------------------

          case e_hashstr64_const( ".lib" ):
            inSources( Type::kLib ).push( path );
            break;

          //--------------------------------------------------------------------
          // Source and header file types.
          //--------------------------------------------------------------------

          case e_hashstr64_const( ".png" ):
            inSources( Type::kPng ).push( path );
            break;
          case e_hashstr64_const( ".inl" ):
            inSources( Type::kInl ).push( path );
            break;
          case e_hashstr64_const( ".hpp" ):
          case e_hashstr64_const( ".hxx" ):
          case e_hashstr64_const( ".hh" ):
            inSources( Type::kHpp ).push( path );
            break;
          case e_hashstr64_const( ".cpp" ):
          case e_hashstr64_const( ".cxx" ):
          case e_hashstr64_const( ".cc" ):
            inSources( Type::kCpp ).push( path );
            break;
          case e_hashstr64_const( ".h" ):
            inSources( Type::kH ).push( path );
            break;
          case e_hashstr64_const( ".c" ):
            inSources( Type::kC ).push( path );
            break;
          default:
            #if 0
              e_warnsf( "Ignoring %s!", ccp( path ));
            #endif
            return;
        }
        #if 0
          e_msgf( "  Found %s", ccp( path ));
        #endif
      }

    //}:                                          |
    //writePropertyGroup:{                        |

      void Workspace::MSVC::writePropGroup( Writer& fs, const string& group, const string& config )const{
        switch( group.hash() ){
          case e_hashstr64_const( "Condition" ):
            fs << "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\" Label=\"Configuration\">\n";
            switch( toBuild().hash() ){
              case e_hashstr64_const( "application" ):
                fs << "  <ConfigurationType>Application</ConfigurationType>\n";
                break;
              case e_hashstr64_const( "console" ):
                fs << "  <ConfigurationType>Console</ConfigurationType>\n";
                break;
            }
            fs << "  <CharacterSet>"+toUnicodeType()+"</CharacterSet>\n";
            fs << "  <PlatformToolset>"+toPlatformTools()+"</PlatformToolset>\n";
            fs << "</PropertyGroup>\n";
            break;
        }
      }

      void Workspace::MSVC::writePropGroup( Writer& fs, const string& group )const{
        if( e_hashstr64_const( "<arch>" ) != group.hash() ){
          fs << "<PropertyGroup Label=\""+group+"\">\n";
        }else{
          fs << "<PropertyGroup>\n";
        }
        switch( group.hash() ){
          case e_hashstr64_const( "UserMacros" ):
            // TODO: Put in user macros here.
            break;
          case e_hashstr64_const( "Globals" ):
            fs << "  <ProjectGuid>"+toProjectGUID()+"</ProjectGuid>\n";
            fs << "  <WindowsTargetPlatformVersion>"+toWindowsSDK()+"</WindowsTargetPlatformVersion>\n";
            fs << "  <Keyword>Win32Proj</Keyword>\n";
            fs << "  <Platform>"+m_sArchitecture+"</Platform>\n";
            fs << "  <ProjectName>"+toLabel()+"</ProjectName>\n";
            fs << "  <VCProjectUpgraderObjectName>NoUpgrade</VCProjectUpgraderObjectName>\n";
            break;
          // Anything in <> is a special case and not sent through to the vcxproj.
          case e_hashstr64_const( "<arch>" ):
            fs << "  <PreferredToolArchitecture>"+toPreferredArch()+"</PreferredToolArchitecture>\n";
            break;
        }
        fs << "</PropertyGroup>\n";
      }

    //}:                                          |
    //writeItemGroup:{                            |

      void Workspace::MSVC::writeItemGroup( Writer& fs, const string& group )const{
        switch( group.hash() ){
          case e_hashstr64_const( "ProjectConfigurations" ):
            fs << "<ItemGroup Label=\"ProjectConfigurations\">\n";
            fs << "  <ProjectConfiguration Include=\"Debug|"+m_sArchitecture+"\">\n";
            fs << "    <Configuration>Debug</Configuration>\n";
            fs << "    <Platform>"+m_sArchitecture+"</Platform>\n";
            fs << "  </ProjectConfiguration>\n";
            fs << "  <ProjectConfiguration Include=\"Release|"+m_sArchitecture+"\">\n";
            fs << "    <Configuration>Release</Configuration>\n";
            fs << "    <Platform>"+m_sArchitecture+"</Platform>\n";
            fs << "  </ProjectConfiguration>\n";
            fs << "</ItemGroup>\n";
            break;
          case e_hashstr64_const( "<source>" ):
            fs << "<ItemGroup>\n";
            toSources().foreach(
              [&]( const Files& files ){
                if( files.empty() ){
                  return;
                }
                auto it = files.getIterator();
                while( it ){
                  if( *it ){
                    if( e_fexists( *it )){
                      const auto& ext = it->ext().tolower();
                      switch( ext.hash() ){
                        case e_hashstr64_const( ".inl" ):
                        case e_hashstr64_const( ".hxx" ):
                        case e_hashstr64_const( ".hh"  ):
                        case e_hashstr64_const( ".hpp" ):
                        case e_hashstr64_const( ".h"   ):
                          break;
                        default:
                          fs << "  <ClCompile Include=\"..\\"+it->os()+"\"/>\n";
                          break;
                      }
                    }
                  }
                  ++it;
                }
              }
            );
            fs << "</ItemGroup>\n";
            break;
        }
      }

    //}:                                          |
    //writeProjVersion:{                          |

      void Workspace::MSVC::writeProjVersion( Writer& fs )const{
        fs << "<_ProjectFileVersion>"+m_sProjectVersion+"</_ProjectFileVersion>\n";
      }

    //}:                                          |
    //writeSetDirectory:{                         |

      void Workspace::MSVC::writeSetDirectory( Writer& fs, const string& tag, const string& config, const string& path )const{
        fs << "<"+tag+" Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">$(SolutionDir)"+path+"\\"+config+"\\</"+tag+">\n";
      }

    //}:                                          |
    //extFromBuildString:{                        |

      ccp Workspace::MSVC::extFromBuildString()const{
        switch( toBuild().hash() ){
          case e_hashstr64_const( "application" ):
            [[fallthrough]];
          case e_hashstr64_const( "console" ):
            return "exe";
          case e_hashstr64_const( "static" ):
            return "lib";
          case e_hashstr64_const( "shared" ):
            return "dll";
        }
        return nullptr;
      }

    //}:                                          |
    //writeManfestData:{                          |

      void Workspace::MSVC::writeManifestData( Writer& fs, const string& config )const{
        fs << "<GenerateManifest Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">"+m_sGenManifest+"</GenerateManifest>\n";
      }

    //}:                                          |
    //writeTargetVar:{                            |

      void Workspace::MSVC::writeTargetVar( Writer& fs, const string& config, const string& tag )const{
        switch( tag.hash() ){
          case e_hashstr64_const( "Name" ):
            fs << "<TargetName Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">"+toLabel()+"</TargetName>\n";
            break;
          case e_hashstr64_const( "Ext" ):
            fs << "<TargetExt Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">."+extFromBuildString()+"</TargetExt>\n";
            break;
        }
      }

    //}:                                          |
    //writeLinkerVar:{                            |

      void Workspace::MSVC::writeLinkerVar( Writer& fs, const string& label, const string& config )const{
        switch( label.hash() ){
          case e_hashstr64_const( "Incremental" ):
            fs << "<LinkIncremental Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">"+m_sLinkIncremental+"</LinkIncremental>\n";
            break;
        }
      }

    //}:                                          |
    //writeImport*:{                              |

      void Workspace::MSVC::writeImportGroup( Writer& fs, const string& label, const string& path )const{
        fs << "<ImportGroup Label=\""+label+"\">\n";
        switch( label.hash() ){
          case e_hashstr64_const( "PropertySheeets" ):
            fs << "<Import Project=\"$(UserRootDir)\\"+path+"\" Condition=\"exists('$(UserRootDir)\\"+path+"')\" Label=\"LocalAppDataPlatform\"/>\n";
            break;
        }
        fs << "</ImportGroup>\n";
      }

      void Workspace::MSVC::writeImportGroup( Writer& fs, const string& label )const{
        fs << "<ImportGroup Label=\""+label+"\">\n";
        switch( label.hash() ){
          case e_hashstr64_const( "ExtensionSettings" ):
            break;
        }
        fs << "</ImportGroup>\n";
      }

      void Workspace::MSVC::writeImport( Writer& fs, const string& type, const string& path )const{
        fs << "<Import "+type+"=\"$(VCTargetsPath)\\"+path+"\"/>\n";
      }

    //}:                                          |
    //wrieItemDefGroup:{                          |

      void Workspace::MSVC::writeItemDefGroup( Writer& fs, const string& config )const{
        fs << "<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='"+config+"|"+m_sArchitecture+"'\">\n";
        fs << "  <ClCompile>\n";
        fs << "    <AdditionalIncludeDirectories>";
        if( !toIncludePaths().empty() ){
          const auto& syspaths = toIncludePaths().splitAtCommas();
          strings paths;
          syspaths.foreach(
            [&]( const string& syspath ){
              if( syspath.empty() ){
                return;
              }
              if( *syspath == '/' ){
                paths.push( syspath );
              }else if( *syspath == '.' ){
                paths.push( syspath );
              }else{
                paths.push( "../" + syspath );
              }
            }
          );
          auto it = paths.getIterator();
          while( it ){
            const auto& s = *it++;
            fs << s + ";";
          }
        }
        fs << "%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n";
        fs << "    <AssemblerListingLocation>$(IntDir)</AssemblerListingLocation>\n";
        fs << "    <BasicRuntimeChecks>"+m_sBasicRuntimeChk+"</BasicRuntimeChecks>\n";
        fs << "    <CompileAs>CompileAsCpp</CompileAs>\n";
        fs << "    <DebugInformationFormat>"+m_sDebugInfoFormat+"</DebugInformationFormat>\n";
        fs << "    <ExceptionHandling>"+m_sExceptionHndlng+"</ExceptionHandling>\n";
        if( !toPrefixHeader().empty() ){
          const auto& prefix = toPrefixHeader();
          fs << "    <ForcedIncludeFiles>"+prefix.os()+"</ForcedIncludeFiles>\n";
        }
        fs << "    <InlineFunctionExpansion>";
        switch( config.hash() ){
          case e_hashstr64_const( "Debug" ):
            fs << "Disabled";
            break;
          case e_hashstr64_const( "Release" ):
            // TODO: Any suitable instead of Disabled.
            fs << "Disabled";
            break;
        }
        fs << "</InlineFunctionExpansion>\n";
        fs << "    <LanguageStandard>";
        switch( toLanguage().hash() ){
          case e_hashstr64_const( "c++17" ):
            fs << "stdcpp17";
            break;
          case e_hashstr64_const( "c++14" ):
            fs << "stdcpp14";
            break;
          case e_hashstr64_const( "c++11" ):
            fs << "stdcpp11";
            break;
        }
        fs << "</LanguageStandard>\n";
        fs << "    <Optimization>";
        switch( config.hash() ){
          case e_hashstr64_const( "Debug" ):
            fs << "Disabled";
            break;
          case e_hashstr64_const( "Release" ):
            // TODO: Heavy optimization here.
            fs << "Disabled";
            break;
        }
        fs << "</Optimization>\n";
        fs << "    <PrecompiledHeader>"+m_sPCH+"</PrecompiledHeader>\n";
        switch( config.hash() ){
          case e_hashstr64_const( "Debug" ):
            fs << "    <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>\n";
            break;
          case e_hashstr64_const( "Release" ):
            fs << "    <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>\n";
            break;
        }
        fs << "    <RuntimeTypeInfo>"+m_sRTTI+"</RuntimeTypeInfo>\n";
        fs << "    <UseFullPaths>false</UseFullPaths>\n";
        fs << "    <WarningLevel>"+m_sWarningLevel+"</WarningLevel>\n";
        fs << "    <PreprocessorDefinitions>";
        switch( config.hash() ){
          case e_hashstr64_const( "Debug" ):/**/{
            string defs = toDefinesDbg() + ";" + toLabel().toupper();
            defs.replace( "\t", "" );
            defs.replace( "\n", "" );
            defs.replace( ",", ";" );
            defs.replace( " ", "" );
            fs << defs + ";";
            break;
          }
          case e_hashstr64_const( "Release" ):/**/{
            string defs = toDefinesRel() + ";" + toLabel().toupper();
            defs.replace( "\t", "" );
            defs.replace( "\n", "" );
            defs.replace( ",", ";" );
            defs.replace( " ", "" );
            fs << defs + ";";
            break;
          }
        }
        fs << "%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
        fs << "    <ObjectFileName>$(IntDir)</ObjectFileName>\n";
        fs << "  </ClCompile>\n";
        fs << "  <Link>\n";
        fs << "    <AdditionalDependencies>kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib</AdditionalDependencies>\n";
        fs << "    <AdditionalLibraryDirectories>%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n";
        fs << "    <AdditionalOptions>%(AdditionalOptions) /machine:"+m_sArchitecture+"</AdditionalOptions>\n";
        fs << "    <GenerateDebugInformation>"+m_sGenReleaseDBInf+"</GenerateDebugInformation>\n";
        fs << "    <IgnoreSpecificDefaultLibraries>%(IgnoreSpecificDefaultLibraries)</IgnoreSpecificDefaultLibraries>\n";
        fs << "    <ProgramDataBaseFile>$(IntDir)"+toLabel()+".pdb</ProgramDataBaseFile>\n";
        switch( toBuild().hash() ){
          case e_hashstr64_const( "application" ):
            fs << "    <SubSystem>Windows</SubSystem>\n";
            break;
          case e_hashstr64_const( "console" ):
            fs << "    <SubSystem>Console</SubSystem>\n";
            break;
        }
        fs << "  </Link>\n";
        fs << "  <ProjectReference>\n";
        fs << "    <LinkLibraryDependencies>"+m_sLinkLibDepends+"</LinkLibraryDependencies>\n";
        fs << "  </ProjectReference>\n";
        fs << "</ItemDefinitionGroup>\n";
      }

    //}:                                          |
    //serialize:{                                 |

      void Workspace::MSVC::serialize( Writer& fs )const{

        //----------------------------------------------------------------------
        // Populate build files across unity space.
        //----------------------------------------------------------------------

        if( !isUnityBuild() ){
          writeProject<MSVC>( fs, Type::kCpp );
          writeProject<MSVC>( fs, Type::kC );
        }else{
          const u32 cores = std::thread::hardware_concurrency();
          u32 i=0;
          const_cast<MSVC*>( this )->toUnity().resize( cores );
          const_cast<MSVC*>( this )->unifyProject<MSVC>( Type::kCpp, i );
          const_cast<MSVC*>( this )->unifyProject<MSVC>( Type::kC,   i );
          writeProject<MSVC>( fs, Type::kCpp );
          writeProject<MSVC>( fs, Type::kC );
        }

        //----------------------------------------------------------------------
        // Save MSVC project to XML format file.
        //----------------------------------------------------------------------

        fs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        fs << "<Project DefaultTargets=\"Build\" ToolsVersion=\"16.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
        writePropGroup( fs, "<arch>" );
        writeItemGroup( fs, "ProjectConfigurations" );
        writeItemGroup( fs, "<source>" );
        writePropGroup( fs, "Globals" );
        writeImport(    fs, "Project", "Microsoft.Cpp.Default.props" );
        for( u32 n=e_dimof( anon_aConfigs ), i=0; i<n; ++i ){
          writePropGroup( fs, "Condition", anon_aConfigs[ i ]);
        }
        writeImport(      fs, "Project", "Microsoft.Cpp.props" );
        writeImportGroup( fs, "ExtensionSettings" );
        writeImportGroup( fs, "PropertySheets", "Microsoft.Cpp.$(Platform).user.props" );
        writePropGroup(   fs, "UserMacros" );
        fs << "<PropertyGroup>\n";
          writeProjVersion(  fs );
          for( u32 n=e_dimof( anon_aConfigs ), i=0; i<n; ++i ){
            writeSetDirectory( fs, "OutDir",      anon_aConfigs[ i ], m_sOutDir );
            writeSetDirectory( fs, "IntDir",      anon_aConfigs[ i ], m_sIntDir );
            writeTargetVar(    fs, "Name",        anon_aConfigs[ i ]);
            writeTargetVar(    fs, "Ext",         anon_aConfigs[ i ]);
            writeLinkerVar(    fs, "Incremental", anon_aConfigs[ i ]);
            writeManifestData( fs, "Debug" );
          }
        fs << "</PropertyGroup>\n";
        for( u32 n=e_dimof( anon_aConfigs ), i=0; i<n; ++i ){
          writeItemDefGroup( fs, anon_aConfigs[ i ]);
        }
        fs << "<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\"/>\n";
        fs << "  <ImportGroup Label=\"ExtensionTargets\">\n";
        fs << "</ImportGroup>\n";
        fs << "</Project>\n";
      }

    //}:                                          |
  //}:                                            |
//}:                                              |
//================================================|=============================

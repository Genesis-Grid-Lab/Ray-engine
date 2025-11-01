#pragma once

#include "Core/Config.h"
#include <filesystem>

namespace RE {

  //asset type
  using AssetID = uint64_t;
  const AssetID EMPTY_ASSET = 0u;

  enum class AssetType : uint8_t{

    UNKNOWN = 0,
    TEXTURE,
    SCENE,
    MODEL,
    SKYBOX,
    SPRTIE,
    SCRIPT,
    SHADER,
  };

  // define the base Asset structure
  struct Asset {
    // generate unique asset id
    AssetID UUID = EMPTY_ASSET;

    // file path of asset
    std::string Source;

    // name of the asset
    std::string Name;

    // asset type
    AssetType Type = AssetType::UNKNOWN;
  };

  struct TextureAsset : Asset{
    Texture2D Data{};
  };

  struct ModelAsset : Asset {
    Model Data{};
  };

  struct SkyboxAsset : Asset {
    Model Data{};
  };

  struct SpriteAsset : Asset {
    // Sprite Data{};
  };

  struct SceneAsset : Asset {
    // to do
  };

  struct ScriptAsset : Asset {
    //to do
  };

  struct ShaderAsset : Asset {
    Shader Data{};
  };

  // define an unordered map to store assets based on their type and UID
  using SharedAsset = Ref<Asset>;
  using AssetMap = std::unordered_map<AssetID, SharedAsset>;

  // asset registry to manage the addition and retrieval of assets
  struct AssetRegistry {
    inline AssetRegistry(){
      // add default asset for each type
      AddEmpty<TextureAsset>();
      AddEmpty<SceneAsset>();
      AddEmpty<ModelAsset>();
      AddEmpty<SkyboxAsset>();
      AddEmpty<SpriteAsset>();
      AddEmpty<ScriptAsset>();
      AddEmpty<ShaderAsset>();
    }

    inline ~AssetRegistry(){
      // best-effort unload; callers should call Clear() / UnloadAll() before CloseWindow()
      try { UnloadAll(); } catch(...) {}
    }

    inline auto AddTexture(AssetID uid, const std::string& source)
    {
      auto asset = CreateRef<TextureAsset>();
      asset->Data = LoadTexture(source.c_str());
      asset->Type = AssetType::TEXTURE;
      Add(uid, source, asset);
      return asset;
    }

    inline auto AddScene(AssetID uid, const std::string& source)
    {
      auto asset = CreateRef<SceneAsset>();
      asset->Type = AssetType::SCENE;
      Add(uid, source, asset);
      return asset;
    }

    inline auto AddModel(AssetID uid, const std::string& source)
    {
      auto asset = CreateRef<ModelAsset>();
      asset->Data = LoadModel(source.c_str());
      asset->Type = AssetType::MODEL;
      Add(uid, source, asset);
      return asset;
    }

    inline auto AddSkybox(AssetID uid, const std::string& source)
    {
      auto asset = CreateRef<SkyboxAsset>();
      Image cubemapImage = LoadImage(source.c_str());    
      // Create cube mesh & model
      bool useHDR = false;
      Mesh cubeSky = GenMeshCube(100.0f, 100.0f, 100.0f);
      asset->Data = LoadModelFromMesh(cubeSky);
      asset->Data.materials[0].shader = AddShader(uid,"Data/Shaders/skybox2.vs", "Data/Shaders/skybox2.fs")->Data;

      int envMap = MATERIAL_MAP_CUBEMAP;
      int doGamma = useHDR ? 1 : 0;
      int vflipped = useHDR ? 1 : 0;
      int zero = 0;
            
      SetShaderValue(asset->Data.materials[0].shader, GetShaderLocation(asset->Data.materials[0].shader, "environmentMap"), &envMap, SHADER_UNIFORM_INT);
      SetShaderValue(asset->Data.materials[0].shader, GetShaderLocation(asset->Data.materials[0].shader, "doGamma"),  &doGamma , SHADER_UNIFORM_INT);
      SetShaderValue(asset->Data.materials[0].shader, GetShaderLocation(asset->Data.materials[0].shader, "vflipped"), &vflipped , SHADER_UNIFORM_INT);
            
      Shader shdrCubemap = AddShader(uid, "Data/Shaders/cubemap.vs", "Data/Shaders/cubemap.fs")->Data;
            
      SetShaderValue(shdrCubemap, GetShaderLocation(shdrCubemap, "equirectangularMap"), &zero , SHADER_UNIFORM_INT);
            
      asset->Data.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(cubemapImage, CUBEMAP_LAYOUT_AUTO_DETECT);
      UnloadImage(cubemapImage);
      asset->Type = AssetType::SKYBOX;
      Add(uid, source, asset);
      return asset;
    }

    inline auto AddSprite(AssetID uid, const std::string& source)
    {
      auto asset = CreateRef<SpriteAsset>();
      // asset->Data.Texture = LoadTexture(source.c_str());
      asset->Type = AssetType::SPRTIE;
      Add(uid, source, asset);
      return asset;
    }

    inline auto AddScript(AssetID uid, const std::string& source)
    {
      auto asset = CreateRef<ScriptAsset>();
      asset->Type = AssetType::SCRIPT;
      Add(uid, source, asset);
      return asset;
    }

    // Add shader asset (vertex & fragment paths concatenated in Source or keep Source as key)
    inline Ref<ShaderAsset> AddShader(AssetID uid, const std::string& vsPath, const std::string& fsPath)
    {
      auto asset = CreateRef<ShaderAsset>();
      asset->Data = LoadShader(vsPath.c_str(), fsPath.c_str());
      asset->Type = AssetType::SHADER;
      // store a combined source for debugging
      std::filesystem::path p(fsPath);
      asset->Source = vsPath + ";" + fsPath;
      asset->Name = p.stem().string();
      asset->UUID = uid;
      mRegistry[TypeID<ShaderAsset>()][asset->UUID] = asset;
      return asset;
    }

    // retrieve asset based on its uid
    template <typename T>
    inline T& Get(AssetID uid)
    {
      const uint32_t type = TypeID<T>();
      if (mRegistry[type].count(uid))
	return (T&)(*mRegistry[type][uid]);
      return (T&)(*mRegistry[type][EMPTY_ASSET]);
    }

    // helps loop through all assets
    template <typename F>
    inline void View(F&& func)
    {
      for(auto& [_, assetMap] : mRegistry)
	{
	  for(auto& [uid, asset] : assetMap)
	    {
	      if(uid != EMPTY_ASSET)
		{
		  func(asset.get());
		}
	    }
	}
    }

    // return collection of asset
    template <typename T>
    inline auto& GetMap()
    {
      return mRegistry[TypeID<T>()];
    }

    // Unload everything safely: Models -> Textures -> Shaders -> Others
    inline void UnloadAll()
    {
      // Unload models first (they reference textures / shaders)
      auto &models = mRegistry[TypeID<ModelAsset>()];
      for (auto &kv : models) {
	if (kv.first == EMPTY_ASSET) continue;
	auto m = std::static_pointer_cast<ModelAsset>(kv.second);
	if (m && m->Data.meshCount > 0) {
	  UnloadModel(m->Data);
	}
      }
      models.clear();

      // Unload textures
      auto &textures = mRegistry[TypeID<TextureAsset>()];
      for (auto &kv : textures) {
	if (kv.first == EMPTY_ASSET) continue;
	auto t = std::static_pointer_cast<TextureAsset>(kv.second);
	if (t && t->Data.id != 0) {
	  UnloadTexture(t->Data);
	}
      }
      textures.clear();

      // Unload sprites' textures
      auto &sprites = mRegistry[TypeID<SpriteAsset>()];
      // for (auto &kv : sprites) {
      //     if (kv.first == EMPTY_ASSET) continue;
      //     auto s = std::static_pointer_cast<SpriteAsset>(kv.second);
      //     if (s && s->Data.Texture.id != 0) {
      //         UnloadTexture(s->Data.Texture);
      //     }
      // }
      sprites.clear();

      auto &skybox = mRegistry[TypeID<SkyboxAsset>()];
      for (auto &kv : skybox) {
	if (kv.first == EMPTY_ASSET) continue;
	auto m = std::static_pointer_cast<ModelAsset>(kv.second);
	if (m && m->Data.meshCount > 0) {
	  UnloadModel(m->Data);
	}
      }
      skybox.clear();

      // Unload shaders last
      auto &shaders = mRegistry[TypeID<ShaderAsset>()];
      for (auto &kv : shaders) {
	if (kv.first == EMPTY_ASSET) continue;
	auto sh = std::static_pointer_cast<ShaderAsset>(kv.second);
	if (sh && sh->Data.id != 0) {
	  UnloadShader(sh->Data);
	}
      }
      shaders.clear();

      // Clear other types
      mRegistry[TypeID<SceneAsset>()].clear();
      mRegistry[TypeID<ScriptAsset>()].clear();
    }

    inline void Clear()
    {
      // Prefer explicit UnloadAll to free GPU resources before context destruction
      UnloadAll();
      mRegistry.clear();
    }

  private:
    // adds a new asset to the registry
    template <typename T>
    inline void Add(
		    AssetID uuid,
		    const std::string& source,
		    Ref<T>& asset
		    )
    {
      asset->UUID = uuid;
      asset->Source = source;
      std::filesystem::path path(source);
      asset->Name = path.stem().string();
      mRegistry[TypeID<T>()][asset->UUID] = asset;
    }

    template <typename T>
    inline void AddEmpty()
    {
      auto empty = CreateRef<T>();
      empty->UUID = EMPTY_ASSET;
      empty->Source = "";
      empty->Name = "";
      empty->Type = AssetType::UNKNOWN;
      mRegistry[TypeID<T>()][EMPTY_ASSET] = empty;
    }
  private:
    std::unordered_map<uint32_t, AssetMap> mRegistry;
  };
}

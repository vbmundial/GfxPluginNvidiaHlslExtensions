using System.Runtime.InteropServices;

public class NvidiaHlslExtensions
{
    public const int ExtensionVersion = 1;
    public const int WarpSize = 32;

    public enum Operations : uint
    {
        Shuffle = 1,
        ShuffleUp = 2,
        ShuffleDown = 3,
        ShuffleXor = 4,

        VoteAll = 5,
        VoteAny = 6,
        VoteBallot = 7,

        GetLaneID = 8,
        Fp16Atomic = 12,
        Fp32Atomic = 13,

        TypedUavLoad = 14
    }

    /// <summary>
    /// Checks whether the shaders of the current Unity game were 
    /// compiled with Nvidia HLSL Shader Extensions enabled or not.
    /// </summary>
    [DllImport("GfxPluginNvidiaHlslExtensions")]
    public static extern bool IsActive();

    /// <summary>
    /// UAV slot index used by Nvidia's driver.
    /// </summary>
    [DllImport("GfxPluginNvidiaHlslExtensions")]
    public static extern int GetUavSlotIndex();

    /// <summary>
    /// Tests if a Shader Extension opcode is supported on current hardware.
    /// </summary>
    [DllImport("GfxPluginNvidiaHlslExtensions")]
    public static extern bool IsOpCodeSupported(Operations opcode);
}
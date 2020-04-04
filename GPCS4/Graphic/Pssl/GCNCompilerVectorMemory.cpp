#include "GCNCompiler.h"

LOG_CHANNEL(Graphic.Pssl.GCNCompilerVectorMemory);

namespace pssl
{;

void GCNCompiler::emitVectorMemory(GCNInstruction& ins)
{
	Instruction::InstructionClass insClass = ins.instruction->GetInstructionClass();

	switch (insClass)
	{
	case Instruction::VectorMemBufNoFmt:
		emitVectorMemBufNoFmt(ins);
		break;
	case Instruction::VectorMemBufFmt:
		emitVectorMemBufFmt(ins);
		break;
	case Instruction::VectorMemImgNoSmp:
		emitVectorMemImgNoSmp(ins);
		break;
	case Instruction::VectorMemImgSmp:
		emitVectorMemImgSmp(ins);
		break;
	case Instruction::VectorMemImgUt:
		emitVectorMemImgUt(ins);
		break;
	case Instruction::VectorMemL1Cache:
		emitVectorMemL1Cache(ins);
		break;
	case Instruction::InstructionClassUnknown:
	case Instruction::InstructionClassCount:
		LOG_FIXME("Instruction class not initialized.");
		break;
	default:
		LOG_ERR("Instruction category is not VectorMemory.");
		break;
	}
}

void GCNCompiler::emitVectorMemBufNoFmt(GCNInstruction& ins)
{
	LOG_PSSL_UNHANDLED_INST();
}

void GCNCompiler::emitVectorMemBufFmt(GCNInstruction& ins)
{
	auto encoding = ins.instruction->GetInstructionFormat();
	if (encoding == Instruction::InstructionSet_MUBUF)
	{
		emitVectorMemBufFmtNoTyped(ins);
	}
	else // MTBUF
	{
		// Typed
		emitVectorMemBufFmtTyped(ins);
	}
}

void GCNCompiler::emitVectorMemBufFmtNoTyped(GCNInstruction& ins)
{
	auto inst = asInst<SIMUBUFInstruction>(ins);

	auto op = inst->GetOp();

	switch (op)
	{
	case SIMUBUFInstruction::BUFFER_LOAD_FORMAT_X:
	case SIMUBUFInstruction::BUFFER_LOAD_FORMAT_XY:
	case SIMUBUFInstruction::BUFFER_LOAD_FORMAT_XYZ:
	case SIMUBUFInstruction::BUFFER_LOAD_FORMAT_XYZW:
		// TODO:
		break;
	case SIMUBUFInstruction::BUFFER_STORE_FORMAT_X:
	case SIMUBUFInstruction::BUFFER_STORE_FORMAT_XY:
	case SIMUBUFInstruction::BUFFER_STORE_FORMAT_XYZ:
	case SIMUBUFInstruction::BUFFER_STORE_FORMAT_XYZW:
		LOG_PSSL_UNHANDLED_INST();
		break;
	default:
		LOG_PSSL_UNHANDLED_INST();
		break;
	}
}

void GCNCompiler::emitVectorMemBufFmtTyped(GCNInstruction& ins)
{
	auto inst = asInst<SIMTBUFInstruction>(ins);
	auto op   = inst->GetOp();

	switch (op)
	{
	case SIMTBUFInstruction::TBUFFER_LOAD_FORMAT_X:
	case SIMTBUFInstruction::TBUFFER_LOAD_FORMAT_XY:
	case SIMTBUFInstruction::TBUFFER_LOAD_FORMAT_XYZ:
	case SIMTBUFInstruction::TBUFFER_LOAD_FORMAT_XYZW:
		// TODO:
		break;
	case SIMTBUFInstruction::TBUFFER_STORE_FORMAT_X:
	case SIMTBUFInstruction::TBUFFER_STORE_FORMAT_XY:
	case SIMTBUFInstruction::TBUFFER_STORE_FORMAT_XYZ:
	case SIMTBUFInstruction::TBUFFER_STORE_FORMAT_XYZW:
		LOG_PSSL_UNHANDLED_INST();
		break;
	default:
		LOG_PSSL_UNHANDLED_INST();
		break;
	}
}

void GCNCompiler::emitVectorMemImgNoSmp(GCNInstruction& ins)
{
	LOG_PSSL_UNHANDLED_INST();
}

void GCNCompiler::emitVectorMemImgSmp(GCNInstruction& ins)
{
	auto inst = asInst<SIMIMGInstruction>(ins);
	auto op = inst->GetOp();

	uint32_t dstReg = inst->GetVDATA();
	uint32_t uvReg = inst->GetVADDR();
	
	// in units of 4 sgprs
	uint32_t sampReg = inst->GetSSAMP() * 4;
	uint32_t texReg = inst->GetSRSRC() * 4;
	
	auto& sampler = m_samplers.at(sampReg);
	auto& texture = m_textures.at(texReg);

	switch (op)
	{
	case SIMIMGInstruction::IMAGE_SAMPLE_L:
	{
		auto u = emitGprLoad<SpirvGprType::Vector>(uvReg);
		auto v = emitGprLoad<SpirvGprType::Vector>(uvReg + 1);

		std::array<uint32_t, 2> uvComponents = { u.id, v.id };
		uint32_t typeId = getVectorTypeId({ SpirvScalarType::Float32, 2 });
		uint32_t uvId = m_module.opCompositeConstruct(typeId, uvComponents.size(), uvComponents.data());

		uint32_t sampledImageId = emitLoadSampledImage(texture, sampler);
		SpirvRegisterValue colorValue;
		colorValue.type.ctype = SpirvScalarType::Float32;
		colorValue.type.ccount = 4; 
		uint32_t resultTypeId = getVectorTypeId(colorValue.type);
		colorValue.id = m_module.opImageSampleImplicitLod(resultTypeId, sampledImageId, uvId, { 0 });
		emitVgprVectorStore(dstReg, colorValue, 0x0F);
	}
		break;
	default:
		LOG_PSSL_UNHANDLED_INST();
		break;
	}
}

void GCNCompiler::emitVectorMemImgUt(GCNInstruction& ins)
{
	LOG_PSSL_UNHANDLED_INST();
}

void GCNCompiler::emitVectorMemL1Cache(GCNInstruction& ins)
{
	LOG_PSSL_UNHANDLED_INST();
}

}  // namespace pssl